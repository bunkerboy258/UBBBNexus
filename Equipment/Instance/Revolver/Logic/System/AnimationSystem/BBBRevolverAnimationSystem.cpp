#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/AnimationSystem/BBBRevolverAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/AnimationSystem/Processors/BBBRevolverAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/AnimationSystem/Processors/BBBRevolverPoseProcessor.h"

void FBBBRevolverAnimationSystem::Update(FBBBRevolverUpdateContext &Context)
{
    FBBBRevolverPoseProcessor::Update(Context);
    FBBBRevolverAnimationProcessor::Update(Context);
}
