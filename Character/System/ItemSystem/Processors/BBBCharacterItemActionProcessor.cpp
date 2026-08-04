
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterItemActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/Results/BBBCharacterItemActionResults.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/Commands/BBBCharacterItemCommands.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/States/BBBCharacterItemStates.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentInstance.h"

void FBBBCharacterItemActionProcessor::Update(
    FBBBCharacterItemCommands &ItemCommands,
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterItemActionResults &ActionResults) const
{
    UBBBEquipmentInstance *ActiveInstance = EquipmentState.GetActiveMainHandInstance();
    ABBBEquipmentActor *EquippedItemActor = ActiveInstance
        ? ActiveInstance->GetModelActor()
        : nullptr;

    if (!EquippedItemActor)
    { return; }

    if (ItemCommands.ConsumeFire())
    {
        if (EquippedItemActor->Fire())
        {
            ActionResults.RecordSuccessfulFire();
        }
    }

    if (ItemCommands.ConsumeReload())
    {
        if (EquippedItemActor->Reload())
        {
            ActionResults.RecordSuccessfulReload();
        }
    }

    if (ItemCommands.ConsumeFirePresentation())
    {
        EquippedItemActor->PresentFire();
    }

    if (ItemCommands.ConsumeReloadPresentation())
    {
        EquippedItemActor->PresentReload();
    }
}
