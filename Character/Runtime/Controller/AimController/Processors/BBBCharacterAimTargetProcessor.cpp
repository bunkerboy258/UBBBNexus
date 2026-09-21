#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Processors/BBBCharacterAimTargetProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/States/BBBAimState.h"

void FBBBCharacterAimTargetProcessor::Update(
    const FBBBCharacterControlState &Control, FBBBAimState &State) const
{
    State.AimTargetWorld = Control.AimTargetWorld;
}
