#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/States/BBBAimStates.h"

void FBBBCharacterAimStateProcessor::Update(
    const FBBBCharacterControlState &ControlData,
    FBBBAimRuntimeState &State) const
{
    // 瞄准或开火意图存在时都保持瞄准状态
    State.bIsAiming = ControlData.WantsAim() || ControlData.WantsFire();
}
