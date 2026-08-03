
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBItemIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Context/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Context/BBBIntentStates.h"

void FBBBItemIntentProcessor::Update(
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
