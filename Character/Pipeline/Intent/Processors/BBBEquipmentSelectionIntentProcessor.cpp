
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBEquipmentSelectionIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBEquipmentSelectionIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    if (Input.bEquipSlot1Pressed)
    {
        Intent.RequestedEquipSlot = 0;

        return;
    }

    if (Input.bEquipSlot2Pressed)
    {
        Intent.RequestedEquipSlot = 1;
    }
}
