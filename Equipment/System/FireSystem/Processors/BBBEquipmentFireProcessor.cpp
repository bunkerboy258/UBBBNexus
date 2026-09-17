#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Processors/BBBEquipmentFireProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Definition/BBBEquipmentFireContext.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Definition/BBBEquipmentReloadRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Fragment/BBBEquipmentFireFragment.h"
#include "Engine/World.h"

void FBBBEquipmentFireProcessor::Update(
    ABBBEquipmentInstance &Instance, USkeletalMeshComponent &WeaponMesh,
    FBBBEquipmentFireRuntimeData &Data, const FBBBEquipmentReloadRuntimeData &Reload,
    const FBBBEquipmentFireFragment &Fragment, FBBBCharacterExternalAPI &CharacterAPI,
    const FName EquipmentId, const bool bIsMirror) const
{
    UWorld *World = Instance.GetWorld();
    if (!ensureMsgf(World, TEXT("[UBBBE]Fire update requires a world")))
    {
        return;
    }

    for (const FBBBEquipmentInput &Input : Data.Inputs)
    {
        if (!bIsMirror && (Reload.bIsReloading
            || !Fragment.CanFire(Data.LoadedAmmo, Data.LastFireTimeSeconds, World->GetTimeSeconds())))
        {
            continue;
        }

        FBBBEquipmentFireContext Context{
            Instance, *World, WeaponMesh, CharacterAPI, Data.LoadedAmmo,
            Data.LastFireTimeSeconds, Input.Sequence, bIsMirror};
        if (!Fragment.Fire(Context))
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Fire failed Equipment=%s Sequence=%d"),
                *EquipmentId.ToString(), Input.Sequence);
            continue;
        }

        Data.FireSequence++;
        if (bIsMirror)
        {
            continue;
        }

        FBBBEquipmentActionEvent Event;
        Event.EquipmentId = EquipmentId;
        Event.Sequence = Input.Sequence;
        Event.ActionType = EBBBCharacterActionType::Fire;
        Event.LoadedAmmo = Data.LoadedAmmo;
        CharacterAPI.PublishEquipmentEvent(Event);
    }
}
