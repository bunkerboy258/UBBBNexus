
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterItemActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemCommands.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemActionResults.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"

void FBBBCharacterItemActionProcessor::Update(
    FBBBCharacterItemCommands &ItemCommands,
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterItemActionResults &ActionResults) const
{
    ABBBEquipmentActor *EquippedItemActor = EquipmentState.GetEquippedItemActor();
    if (!EquippedItemActor)
    {
        return;
    }
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
