#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/BBBMinigunAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/Processors/BBBMinigunAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/AnimationSystem/Processors/BBBMinigunPoseProcessor.h"

void FBBBMinigunAnimationSystem::Update(FBBBMinigunUpdateContext &Context)
{
    FBBBMinigunPoseProcessor::Update(Context);
    FBBBMinigunAnimationProcessor::Update(Context);
}
