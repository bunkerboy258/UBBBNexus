
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBEquipmentIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/BBBIntentPipelineTypes.h"

void FBBBEquipmentIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    Intent.bWantsFire = Input.bFireHeld;
    Intent.bWantsReload = Input.bReloadPressed;
}
