#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBControlBehaviors.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBMoveBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return !Input.World.ContainsNaN();
}

void FBBBMoveBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.Control.MoveWorld = Input.World.GetClampedToMaxSize(1.0f);
}

bool FBBBViewBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return !Input.FacingWorld.ContainsNaN() && !Input.AimTargetWorld.ContainsNaN();
}

void FBBBViewBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.Control.FacingWorld = Input.FacingWorld;
    Data.Operation.Control.AimTargetWorld = Input.AimTargetWorld;
}

bool FBBBAimBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return true;
}

void FBBBAimBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.Control.bAim = Input.bHeld;
    if (Input.bHeld)
    {
        Data.Operation.ActiveBehaviorGroups |= BBBBehaviorGroup::Aim;
    }
}

bool FBBBWalkBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return true;
}

void FBBBWalkBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.Control.bWalk = Input.bHeld;
}

bool FBBBCrouchBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return true;
}

void FBBBCrouchBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.Control.bCrouch = Input.bHeld;
}

bool FBBBJumpBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return true;
}

void FBBBJumpBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.Control.bJump = true;
}

bool FBBBSprintBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    return true;
}

void FBBBSprintBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.Control.bSprint = Input.bHeld;
}
