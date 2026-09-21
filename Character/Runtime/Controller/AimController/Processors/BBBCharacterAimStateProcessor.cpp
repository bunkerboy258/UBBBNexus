#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/States/BBBAimStates.h"

void FBBBCharacterAimStateProcessor::Update(
    const FBBBCharacterControlState &ControlData,
    FBBBAimRuntimeState &State) const
{
    // 瞄准或开火意图存在时都保持瞄准状态
    State.bIsAiming = ControlData.Value.bAim || ControlData.Value.bFire;
}
