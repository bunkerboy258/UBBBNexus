#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Processors/BBBEquipmentInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/Definition/BBBEquipmentInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Definition/BBBEquipmentEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Definition/BBBEquipmentReloadRuntimeData.h"

void FBBBEquipmentInputProcessor::Update(
    FBBBEquipmentInputRuntimeData &Input, FBBBEquipmentEquipRuntimeData &Equip,
    FBBBEquipmentFireRuntimeData &Fire, FBBBEquipmentReloadRuntimeData &Reload,
    const FName EquipmentId, const bool bIsMirror) const
{
    // 取出本帧输入并清空原始队列
    TArray<FBBBEquipmentInput> Pending = MoveTemp(Input.Pending);
    Input.Pending.Reset();
    // 按输入顺序更新装备各系统状态
    for (FBBBEquipmentInput &Entry : Pending)
    {
        if (Entry.Type == EBBBEquipmentInputType::Snapshot)
        {
            const FBBBEquipmentActionFact &Snapshot = Entry.Snapshot;
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
            // 按事实身份恢复装备动作与换弹阶段
            switch (Snapshot.PacketId)
            {
            case FBBBEquipFactPacket::PacketId:
                Entry.Type = EBBBEquipmentInputType::Equip;
                Equip.Inputs.Add(Entry);
                break;
            case FBBBFireFactPacket::PacketId:
                Entry.Type = EBBBEquipmentInputType::Fire;
                Fire.Inputs.Add(Entry);
                break;
            case FBBBReloadStartedFactPacket::PacketId:
                Reload.bIsReloading = true;
                Reload.bMagazineDetached = false;
                Reload.Sequence = Snapshot.Sequence;
                Entry.Type = EBBBEquipmentInputType::Reload;
                Reload.Inputs.Add(Entry);
                break;
            case FBBBMagazineDetachedFactPacket::PacketId:
                Reload.bMagazineDetached = true;
                break;
            case FBBBMagazineLoadedFactPacket::PacketId:
                Reload.bIsReloading = false;
                break;
            case FBBBReloadCancelledFactPacket::PacketId:
                Reload.bIsReloading = false;
                Reload.Inputs.RemoveAll([&Snapshot](const FBBBEquipmentInput &Queued)
                {
                    return Queued.Sequence == Snapshot.Sequence;
                });
                break;
            default:
                ensureMsgf(false, TEXT("[UBBBE]Unknown snapshot packet id=%d"), Snapshot.PacketId);
                break;
            }

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
