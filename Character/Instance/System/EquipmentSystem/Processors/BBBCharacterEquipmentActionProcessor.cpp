#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

void FBBBCharacterEquipmentActionProcessor::Update(
    FBBBCharacterEquipmentCommands &Commands, FBBBCharacterEquipmentState &State) const
{
    ABBBEquipment *Equipment = State.GetActiveMainHandInstance();
    TArray<FBBBEquipmentActionEvent> Restored = Commands.ConsumeRestoredActions();
    const bool bFire = Commands.ConsumeFire();
    const bool bReload = Commands.ConsumeReload();
    if (!Equipment)
    {
        return;
    }

    FBBBEquipmentExternalAPI &API = Equipment->GetExternalAPI();
    for (const FBBBEquipmentActionEvent &Event : Restored)
    {
        if (Event.EquipmentId == Equipment->GetEquipmentId())
        {
            API.ApplySnapshot(Event);
        }
    }

    for (const FBBBCharacterReloadAnimationInput &Input : Commands.ReloadInputs)
    {
        switch (Input.Phase)
        {
        case EBBBCharacterReloadAnimationPhase::Start:
            API.SubmitDetachMagazine(Input.Sequence);
            break;
        case EBBBCharacterReloadAnimationPhase::End:
            API.SubmitLoadMagazine(Input.Sequence);
            break;
        case EBBBCharacterReloadAnimationPhase::Interrupted:
            API.SubmitCancelReload(Input.Sequence);
            break;
        }
    }

    if (bFire)
    {
        API.SubmitFire(State.NextActionSequence++);
    }
    if (bReload)
    {
        API.SubmitReload(State.NextActionSequence++);
    }
}
