
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBEquipmentActionIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBEquipmentActionIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    Intent.bWantsFire = Input.bFireHeld;
    Intent.bWantsReload = Input.bReloadPressed;
}
