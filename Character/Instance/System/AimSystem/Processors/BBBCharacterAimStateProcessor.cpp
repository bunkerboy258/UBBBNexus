#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AimSystem/Definition/States/BBBAimStates.h"

void FBBBCharacterAimStateProcessor::Update(
    const FBBBIntentRuntimeData &IntentData,
    FBBBAimRuntimeState &State) const
{
    State.bIsAiming = IntentData.WantsAim() || IntentData.WantsFire();
}
