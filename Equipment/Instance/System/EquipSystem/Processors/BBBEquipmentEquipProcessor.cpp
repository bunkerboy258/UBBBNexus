#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Processors/BBBEquipmentEquipProcessor.h"

#include "BBBWork/UBBBNexus/Character/Instance/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Definition/BBBEquipmentEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Definition/BBBEquipmentFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBEquipmentEquipFragment.h"

void FBBBEquipmentEquipProcessor::Update(
    FBBBEquipmentEquipRuntimeData &Data, const FBBBEquipmentFireRuntimeData &Fire,
    const FBBBEquipmentEquipFragment &Fragment, FBBBCharacterExternalAPI &CharacterAPI,
    const FName EquipmentId, const bool bIsMirror) const
{
    for (const FBBBEquipmentInput &Input : Data.Inputs)
    {
        if (!Fragment.SubmitMontage(CharacterAPI, Input.Sequence))
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBBBE]Equip presentation failed Equipment=%s Sequence=%d"),
                *EquipmentId.ToString(), Input.Sequence);
            continue;
        }

        if (bIsMirror)
        {
            continue;
        }

        FBBBEquipmentActionEvent Event;
        Event.EquipmentId = EquipmentId;
        Event.Sequence = Input.Sequence;
        Event.ActionType = EBBBCharacterActionType::Equip;
        Event.LoadedAmmo = Fire.LoadedAmmo;
        CharacterAPI.PublishEquipmentEvent(Event);
    }
}
