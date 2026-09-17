#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Processors/BBBCharacterAimTargetProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/States/BBBAimStates.h"

void FBBBCharacterAimTargetProcessor::Update(
    const FBBBCharacterControlState &Control, FBBBAimRuntimeState &State) const
{
    State.AimTargetWorld = Control.GetAimTargetWorld();
}
