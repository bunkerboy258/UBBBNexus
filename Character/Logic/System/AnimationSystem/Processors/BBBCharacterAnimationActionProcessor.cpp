#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"

void FBBBCharacterAnimationActionProcessor::Update(
    FBBBCharacterAnimationUpdateContext &Context) const
{
    Context.AnimationInstance.UpdateMontageContributions();
}
