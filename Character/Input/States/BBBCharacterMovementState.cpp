#include "BBBWork/UBBBNexus/Character/Input/States/BBBCharacterMovementState.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"

void FBBBCharacterMovementState::Apply(FBBBCharacterControlFacts &Control) const
{
    Control.MoveWorld = MoveWorld;
    Control.FacingWorld = FacingWorld;
    Control.bWalk = bWalk;
    Control.bSprint = bSprint;
    Control.bCrouch = bCrouch;
}
