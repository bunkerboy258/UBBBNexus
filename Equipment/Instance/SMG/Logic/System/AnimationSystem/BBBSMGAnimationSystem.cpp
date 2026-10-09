#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/BBBSMGAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/Processors/BBBSMGAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/AnimationSystem/Processors/BBBSMGPoseProcessor.h"

void FBBBSMGAnimationSystem::Update(FBBBSMGUpdateContext &Context)
{
    FBBBSMGPoseProcessor::Update(Context);
    FBBBSMGAnimationProcessor::Update(Context);
}
