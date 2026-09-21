#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/States/BBBAimState.h"

void FBBBCharacterAimStateProcessor::Update(
    const FBBBCharacterControlState &ControlData,
    FBBBAimState &State) const
{
    // 瞄准或开火意图存在时都保持瞄准状态
    State.bIsAiming = ControlData.bAim || ControlData.bFire;
}
