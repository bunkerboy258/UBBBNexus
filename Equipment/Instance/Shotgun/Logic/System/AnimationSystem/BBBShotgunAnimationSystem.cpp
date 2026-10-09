#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/BBBShotgunAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/Processors/BBBShotgunAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/Processors/BBBShotgunPoseProcessor.h"

void FBBBShotgunAnimationSystem::Update(FBBBShotgunUpdateContext &Context)
{
    FBBBShotgunPoseProcessor::Update(Context);
    FBBBShotgunAnimationProcessor::Update(Context);
}
