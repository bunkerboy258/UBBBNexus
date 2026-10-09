#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/BBBSniperAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/Processors/BBBSniperAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/AnimationSystem/Processors/BBBSniperPoseProcessor.h"

void FBBBSniperAnimationSystem::Update(FBBBSniperUpdateContext &Context)
{
    FBBBSniperPoseProcessor::Update(Context);
    FBBBSniperAnimationProcessor::Update(Context);
}
