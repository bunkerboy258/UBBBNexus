#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/DomainData/States/BBBAimState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeState.h"

void FBBBCharacterAimStateProcessor::Update(
    const FBBBCharacterControlState &ControlData,
    FBBBAimState &State,
    const FBBBCharacterLifeState &Life) const
{
    State.bIsAiming = Life.bActionsAllowed && ControlData.bAim;
}
