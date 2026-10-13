#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/AnimationSystem/BBBRifleAnimationSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRifleAnimationProcessor.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRiflePoseProcessor.h"

void FBBBRifleAnimationSystem::Update(FBBBRifleUpdateContext &Context)
{
    FBBBRiflePoseProcessor::Update(Context);
    FBBBRifleAnimationProcessor::Update(Context);
}
