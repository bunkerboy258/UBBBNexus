#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Processors/BBBEquipmentReloadProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Definition/BBBEquipmentReloadRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Definition/BBBEquipmentReloadContext.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Fragment/BBBEquipmentReloadFragment.h"

void FBBBEquipmentReloadProcessor::Update(
    FBBBEquipmentReloadRuntimeData &Data, FBBBEquipmentFireRuntimeData &Fire,
    const FBBBEquipmentReloadFragment &Fragment, FBBBCharacterExternalAPI &CharacterAPI,
    const FName EquipmentId, const bool bIsMirror) const
{
    for (const FBBBEquipmentInput &Input : Data.Inputs)
    {
        const bool bCancelPending = Input.Type == EBBBEquipmentInputType::CancelPendingActions;
        const int32 Sequence = bCancelPending ? Data.Sequence : Input.Sequence;
        FBBBEquipmentReloadContext Context{
            CharacterAPI, Fire.LoadedAmmo, Fire.AmmoCapacity, Data.bIsReloading,
            Data.bMagazineDetached, Data.Sequence, Sequence, bIsMirror};
        if (bIsMirror)
        {
            if (!Fragment.Begin(Context))
            {
                UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Mirror reload presentation failed Sequence=%d"), Sequence);
            }
            continue;
        }

        FBBBEquipmentActionEvent Event;
        Event.EquipmentId = EquipmentId;
        Event.Sequence = Sequence;
        Event.ActionType = EBBBCharacterActionType::Reload;

        if (Input.Type == EBBBEquipmentInputType::Reload)
        {
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

            Event.Phase = EBBBCharacterEquipmentPhase::ReloadStarted;
            Event.LoadedAmmo = Fire.LoadedAmmo;
            CharacterAPI.PublishEquipmentEvent(Event);
            continue;
        }

        if (!Data.bIsReloading || Data.Sequence != Sequence)
        {
            if (!bCancelPending)
            {
                UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Rejected stale reload input Type=%d Sequence=%d"),
                    static_cast<int32>(Input.Type), Sequence);
            }
            continue;
        }

        bool bSucceeded = false;
        switch (Input.Type)
        {
        case EBBBEquipmentInputType::DetachMagazine:
            if (!Data.bMagazineDetached)
            {
                bSucceeded = Fragment.DetachMagazine(Context);
                Event.Phase = EBBBCharacterEquipmentPhase::MagazineDetached;
            }
            break;
        case EBBBEquipmentInputType::LoadMagazine:
            if (Data.bMagazineDetached)
            {
                bSucceeded = Fragment.LoadMagazine(Context);
                Event.Phase = EBBBCharacterEquipmentPhase::MagazineLoaded;
            }
            break;
        case EBBBEquipmentInputType::CancelReload:
        case EBBBEquipmentInputType::CancelPendingActions:
            bSucceeded = Fragment.Cancel(Context);
            Event.Phase = EBBBCharacterEquipmentPhase::ReloadCancelled;
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

        Event.LoadedAmmo = Fire.LoadedAmmo;
        CharacterAPI.PublishEquipmentEvent(Event);
    }
}
