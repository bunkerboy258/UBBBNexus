#include "BBBWork/UBBBNexus/Equipment/Pipeline/Input/Processors/BBBEquipmentInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Input/Definition/BBBEquipmentInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/Definition/BBBEquipmentEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Definition/BBBEquipmentReloadRuntimeData.h"

void FBBBEquipmentInputProcessor::Update(
    FBBBEquipmentInputRuntimeData &Input, FBBBEquipmentEquipRuntimeData &Equip,
    FBBBEquipmentFireRuntimeData &Fire, FBBBEquipmentReloadRuntimeData &Reload,
    FBBBCharacterExternalAPI &CharacterAPI, const FName EquipmentId, const bool bIsMirror) const
{
    TArray<FBBBEquipmentInput> Pending = MoveTemp(Input.Pending);
    Input.Pending.Reset();
    for (FBBBEquipmentInput &Entry : Pending)
    {
        if (Entry.Type == EBBBEquipmentInputType::Snapshot)
        {
            const FBBBEquipmentActionEvent &Snapshot = Entry.Snapshot;
            if (!ensureMsgf(bIsMirror && Snapshot.EquipmentId == EquipmentId && Snapshot.Sequence > 0,
                TEXT("[UBBBE]Invalid mirror input Equipment=%s Sequence=%d"),
                *EquipmentId.ToString(), Snapshot.Sequence))
            {
                continue;
            }

            Fire.LoadedAmmo = Snapshot.LoadedAmmo;
            Entry.Sequence = Snapshot.Sequence;
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

            CharacterAPI.PublishEquipmentEvent(Snapshot);
            continue;
        }

        if (!ensureMsgf(!bIsMirror, TEXT("[UBBBE]Mirror input cannot request local operations")))
        {
            continue;
        }

        if (Entry.Type == EBBBEquipmentInputType::CancelPendingActions)
        {
            Equip.Inputs.Reset();
            Fire.Inputs.Reset();
            Reload.Inputs.Reset();
            Reload.Inputs.Add(Entry);
            continue;
        }

        if (!ensureMsgf(Entry.Sequence > 0, TEXT("[UBBBE]Input sequence must be positive")))
        {
            continue;
        }

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
            ensureMsgf(false, TEXT("[UBBBE]Unknown equipment input type=%d"), static_cast<int32>(Entry.Type));
            break;
        }
    }
}
