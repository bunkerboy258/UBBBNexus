#include "BBBWork/UBBBNexus/Character/Logic/Controller/AimController/Processors/BBBCharacterAimTargetProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/AimController/DomainData/States/BBBAimState.h"

void FBBBCharacterAimTargetProcessor::Update(
    const FBBBCharacterControlState &Control, FBBBAimState &State) const
{
    State.AimTargetWorld = Control.AimTargetWorld;
}
