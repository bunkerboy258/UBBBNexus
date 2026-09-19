#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Processors/BBBEquipmentReloadProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Definition/BBBEquipmentReloadRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Definition/BBBEquipmentReloadContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Fragment/BBBEquipmentReloadFragment.h"

void FBBBEquipmentReloadProcessor::Update(
    FBBBEquipmentReloadRuntimeData &Data, FBBBEquipmentFireRuntimeData &Fire,
    const FBBBEquipmentReloadFragment &Fragment, ABBBCharacter &CharacterAPI,
    const FName EquipmentId, const bool bIsMirror) const
{
    // 按输入顺序处理换弹阶段
    for (const FBBBEquipmentInput &Input : Data.Inputs)
    {
        const bool bCancelPending = Input.Type == EBBBEquipmentInputType::CancelPendingActions;
        const int32 Sequence = bCancelPending ? Data.Sequence : Input.Sequence;
        FBBBEquipmentReloadContext Context{
            CharacterAPI, Fire.LoadedAmmo, Fire.AmmoCapacity, Data.bIsReloading,
            Data.bMagazineDetached, Data.Sequence, Sequence, bIsMirror};
        // 镜像实例只重放换弹开始表现
        if (bIsMirror)
        {
            if (!Fragment.Begin(Context))
            {
                UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Mirror reload presentation failed Sequence=%d"), Sequence);
            }
            continue;
        }

        if (Input.Type == EBBBEquipmentInputType::Reload)
        {
            // 换弹开始前确认当前状态和弹药条件
            if (Data.bIsReloading || !Fragment.CanReload(Fire.LoadedAmmo, Fire.AmmoCapacity))
            {
                continue;
            }

            if (!Fragment.Begin(Context))
            {
                UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Reload start failed Equipment=%s Sequence=%d"),
                    *EquipmentId.ToString(), Sequence);
                continue;
            }

            // 发布换弹开始事实
            FBBBReloadStartedFactPacket Fact;
            Fact.EquipmentId = EquipmentId;
            Fact.Sequence = Sequence;
            Fact.LoadedAmmo = Fire.LoadedAmmo;
            CharacterAPI.SubmitInput(Fact);
            continue;
        }

        // 后续换弹阶段必须匹配当前换弹序号
        if (!Data.bIsReloading || Data.Sequence != Sequence)
        {
            if (!bCancelPending)
            {
                UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Rejected stale reload input Type=%d Sequence=%d"),
                    static_cast<int32>(Input.Type), Sequence);
            }
            continue;
        }

        // 根据输入执行脱匣装匣或取消阶段 事实包类型与阶段一一对应
        bool bSucceeded = false;
        switch (Input.Type)
        {
        case EBBBEquipmentInputType::DetachMagazine:
            if (!Data.bMagazineDetached)
            {
                bSucceeded = Fragment.DetachMagazine(Context);
                if (bSucceeded)
                {
                    FBBBMagazineDetachedFactPacket Fact;
                    Fact.EquipmentId = EquipmentId;
                    Fact.Sequence = Sequence;
                    Fact.LoadedAmmo = Fire.LoadedAmmo;
                    CharacterAPI.SubmitInput(Fact);
                }
            }
            break;
        case EBBBEquipmentInputType::LoadMagazine:
            if (Data.bMagazineDetached)
            {
                bSucceeded = Fragment.LoadMagazine(Context);
                if (bSucceeded)
                {
                    FBBBMagazineLoadedFactPacket Fact;
                    Fact.EquipmentId = EquipmentId;
                    Fact.Sequence = Sequence;
                    Fact.LoadedAmmo = Fire.LoadedAmmo;
                    CharacterAPI.SubmitInput(Fact);
                }
            }
            break;
        case EBBBEquipmentInputType::CancelReload:
        case EBBBEquipmentInputType::CancelPendingActions:
            bSucceeded = Fragment.Cancel(Context);
            if (bSucceeded)
            {
                FBBBReloadCancelledFactPacket Fact;
                Fact.EquipmentId = EquipmentId;
                Fact.Sequence = Sequence;
                Fact.LoadedAmmo = Fire.LoadedAmmo;
                CharacterAPI.SubmitInput(Fact);
            }
            break;
        default:
            break;
        }

        if (!bSucceeded)
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Rejected reload stage Type=%d Sequence=%d"),
                static_cast<int32>(Input.Type), Sequence);
            continue;
        }
    }
}
