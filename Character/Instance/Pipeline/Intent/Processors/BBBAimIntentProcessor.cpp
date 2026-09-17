
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Processors/BBBAimIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBAimIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    Intent.bWantsAim = Input.bPrecisionAimHeld || Input.bFireRaisedGraceActive;
}
