#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"

void FBBBCharacterAnimationActionProcessor::Update(
    FBBBCharacterAnimationUpdateContext &Context) const
{
    Context.AnimationInstance.UpdateApprovedMontages();
}
