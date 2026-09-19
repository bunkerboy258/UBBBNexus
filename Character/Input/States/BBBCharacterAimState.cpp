#include "BBBWork/UBBBNexus/Character/Input/States/BBBCharacterAimState.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"

void FBBBCharacterAimState::Apply(FBBBCharacterControlFacts &Control) const
{
    Control.AimTargetWorld = AimTargetWorld;
    Control.bAim = bAim;
}
