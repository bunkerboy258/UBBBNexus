#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/AnimationSystem/BBBPistolAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/AnimationSystem/Processors/BBBPistolAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/AnimationSystem/Processors/BBBPistolPoseProcessor.h"

void FBBBPistolAnimationSystem::Update(FBBBPistolUpdateContext &Context)
{
    FBBBPistolPoseProcessor::Update(Context);
    FBBBPistolAnimationProcessor::Update(Context);
}
