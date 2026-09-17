
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Processors/BBBInputIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBInputIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    Intent.MoveInput = Input.MoveInput;
    Intent.bHasMoveInput = Input.bHasMoveInput;
    Intent.bWantsWalk = Input.bWalkHeld && !Input.bSprintHeld;
    Intent.bWantsSprint = Input.bSprintHeld;
    Intent.bWantsCrouch = Input.bCrouchHeld;
    Intent.bWantsJump = Input.bJumpPressed;
    Intent.bWantsDash = Input.bDashPressed;
    Intent.bWantsSlide = Input.bSlidePressed;
}
