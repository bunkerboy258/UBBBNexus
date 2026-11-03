#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/Core/Update/BBBMaodieTestUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/BBBMaodieTestBehaviorSystem.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AnimationSystem/BBBMaodieTestAnimationSystem.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/NetworkSystem/BBBMaodieTestNetworkSystem.h"
void FBBBMaodieTestUpdatePipeline::Update(ABBBMaodieTest &Actor, float DeltaSeconds)
{
    const bool bCausal = Actor.HasAuthority();
    FBBBMaodieTestParseSystem::Update(Actor, DeltaSeconds, bCausal);
    FBBBMaodieTestBehaviorSystem::Update(Actor, DeltaSeconds, bCausal);
    FBBBMaodieTestAnimationSystem::Update(Actor, DeltaSeconds, bCausal);
    FBBBMaodieTestNetworkSystem::Update(Actor, DeltaSeconds, bCausal);
}
