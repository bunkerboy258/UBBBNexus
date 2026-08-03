
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBEquipmentIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Context/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Context/BBBIntentStates.h"

void FBBBEquipmentIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    Intent.bWantsFire = Input.bFireHeld;
    Intent.bWantsReload = Input.bReloadPressed;
}
