#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AnimationSystem/Processors/BBBMaodieTestAnimationProcessor.h"
/** 耄耋Animation调度根 */
class FBBBMaodieTestAnimationSystem final
{
public:
    /** @param Actor 耄耋 @param DeltaSeconds 帧间隔 @param bCausal 业务控制身份 @return 无 */
    static void Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
    {
        FBBBMaodieTestAnimationProcessor::Update(Actor, DeltaSeconds, bCausal);
    }
};
