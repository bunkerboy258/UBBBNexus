
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBAimIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Context/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Context/BBBIntentStates.h"

void FBBBAimIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    Intent.bWantsAim = Input.bPrecisionAimHeld || Input.bFireHeld || Input.bFireRaisedGraceActive;
}
