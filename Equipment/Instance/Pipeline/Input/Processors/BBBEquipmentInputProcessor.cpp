#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Processors/BBBEquipmentInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Definition/BBBEquipmentEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Definition/BBBEquipmentReloadRuntimeData.h"

void FBBBEquipmentInputProcessor::Update(
    FBBBEquipmentInputRuntimeData &Input, FBBBEquipmentEquipRuntimeData &Equip,
    FBBBEquipmentFireRuntimeData &Fire, FBBBEquipmentReloadRuntimeData &Reload,
    FBBBCharacterInput &CharacterAPI, const FName EquipmentId, const bool bIsMirror) const
{
    // 取出本帧输入并清空原始队列
    TArray<FBBBEquipmentInput> Pending = MoveTemp(Input.Pending);
    Input.Pending.Reset();
    // 按输入顺序更新装备各系统状态
    for (FBBBEquipmentInput &Entry : Pending)
    {
        if (Entry.Type == EBBBEquipmentInputType::Snapshot)
        {
            const FBBBEquipmentActionEvent &Snapshot = Entry.Snapshot;
            // 恢复快照必须匹配镜像装备和当前实例
            if (!ensureMsgf(bIsMirror && Snapshot.EquipmentId == EquipmentId && Snapshot.Sequence > 0,
                TEXT("[UBBBE]Invalid mirror input Equipment=%s Sequence=%d"),
                *EquipmentId.ToString(), Snapshot.Sequence))
            {
                continue;
            }

            // 恢复网络弹药状态
            Fire.LoadedAmmo = Snapshot.LoadedAmmo;
            Entry.Sequence = Snapshot.Sequence;
            // 恢复装备动作时重新进入对应输入队列
            if (Snapshot.ActionType == EBBBCharacterActionType::Equip)
            {
                Entry.Type = EBBBEquipmentInputType::Equip;
                Equip.Inputs.Add(Entry);
            }
            if (Snapshot.ActionType == EBBBCharacterActionType::Fire)
            {
                Entry.Type = EBBBEquipmentInputType::Fire;
                Fire.Inputs.Add(Entry);
            }
            // 根据恢复事件阶段更新换弹状态
            switch (Snapshot.Phase)
            {
            case EBBBCharacterEquipmentPhase::ReloadStarted:
                Reload.bIsReloading = true;
                Reload.bMagazineDetached = false;
                Reload.Sequence = Snapshot.Sequence;
                Entry.Type = EBBBEquipmentInputType::Reload;
                Reload.Inputs.Add(Entry);
                break;
            case EBBBCharacterEquipmentPhase::MagazineDetached:
                Reload.bMagazineDetached = true;
                break;
            case EBBBCharacterEquipmentPhase::MagazineLoaded:
                Reload.bIsReloading = false;
                break;
            case EBBBCharacterEquipmentPhase::ReloadCancelled:
                Reload.bIsReloading = false;
                Reload.Inputs.RemoveAll([&Snapshot](const FBBBEquipmentInput &Queued)
                {
                    return Queued.Sequence == Snapshot.Sequence;
                });
                break;
            default:
                break;
            }

            // 将恢复事件发布到角色外部接口
            FBBBCharacterDiscreteInput CharacterInput;
            CharacterInput.Equipment.ActionEvent = Snapshot;
            CharacterAPI.Submit(CharacterInput);
            continue;
        }

        // 非镜像装备只接受本地操作输入
        if (!ensureMsgf(!bIsMirror, TEXT("[UBBBE]Mirror input cannot request local operations")))
        {
            continue;
        }

        if (Entry.Type == EBBBEquipmentInputType::CancelPendingActions)
        {
            // 取消待处理动作并保留取消输入
            Equip.Inputs.Reset();
            Fire.Inputs.Reset();
            Reload.Inputs.Reset();
            Reload.Inputs.Add(Entry);
            continue;
        }

        // 普通装备操作必须携带有效序号
        if (!ensureMsgf(Entry.Sequence > 0, TEXT("[UBBBE]Input sequence must be positive")))
        {
            continue;
        }

        // 将输入路由到对应装备系统
        switch (Entry.Type)
        {
        case EBBBEquipmentInputType::Equip:
            Equip.Inputs.Add(Entry);
            break;
        case EBBBEquipmentInputType::Fire:
            Fire.Inputs.Add(Entry);
            break;
        case EBBBEquipmentInputType::Reload:
        case EBBBEquipmentInputType::DetachMagazine:
        case EBBBEquipmentInputType::LoadMagazine:
        case EBBBEquipmentInputType::CancelReload:
            Reload.Inputs.Add(Entry);
            break;
        default:
            // 未知输入类型触发防呆报警
            ensureMsgf(false, TEXT("[UBBBE]Unknown equipment input type=%d"), static_cast<int32>(Entry.Type));
            break;
        }
    }
}
