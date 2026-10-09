#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/AnimationSystem/BBBLMGAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/AnimationSystem/Processors/BBBLMGAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/AnimationSystem/Processors/BBBLMGPoseProcessor.h"

void FBBBLMGAnimationSystem::Update(FBBBLMGUpdateContext &Context)
{
    FBBBLMGPoseProcessor::Update(Context);
    FBBBLMGAnimationProcessor::Update(Context);
}
