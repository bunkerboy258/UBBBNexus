#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/Processors/BBBMaodieTestBehaviorProcessor.h"
/** 耄耋Behavior调度根 */
class FBBBMaodieTestBehaviorSystem final
{
public:
    /** @param Actor 耄耋 @param DeltaSeconds 帧间隔 @param bCausal 业务控制身份 @return 无 */
    static void Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
    {
        FBBBMaodieTestBehaviorProcessor::Update(Actor, DeltaSeconds, bCausal);
    }
};
