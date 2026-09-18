#include "BBBWork/UBBBNexus/Character/Instance/Controller/AimController/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Instance/Controller/AimController/Definition/States/BBBAimStates.h"

void FBBBCharacterAimStateProcessor::Update(
    const FBBBCharacterControlState &ControlData,
    FBBBAimRuntimeState &State) const
{
    // 瞄准或开火意图存在时都保持瞄准状态
    State.bIsAiming = ControlData.WantsAim() || ControlData.WantsFire();
}
