
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBInputIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Context/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Context/BBBIntentStates.h"

void FBBBInputIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    Intent.MoveInput = Input.MoveInput;
    Intent.LookDelta = Input.LookDelta;
    Intent.bHasMoveInput = Input.bHasMoveInput;
}
