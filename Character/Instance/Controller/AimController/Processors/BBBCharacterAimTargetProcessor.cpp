#include "BBBWork/UBBBNexus/Character/Instance/Controller/AimController/Processors/BBBCharacterAimTargetProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Instance/Controller/AimController/Definition/States/BBBAimStates.h"

void FBBBCharacterAimTargetProcessor::Update(
    const FBBBCharacterControlState &Control, FBBBAimRuntimeState &State) const
{
    State.AimTargetWorld = Control.GetAimTargetWorld();
}
