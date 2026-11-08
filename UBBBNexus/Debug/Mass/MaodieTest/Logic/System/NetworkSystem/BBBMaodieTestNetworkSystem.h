#pragma once
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/NetworkSystem/Processors/BBBMaodieTestNetworkProcessor.h"
/** 耄耋Network调度根 */
class FBBBMaodieTestNetworkSystem final
{
public:
    /** @param Actor 耄耋 @param DeltaSeconds 帧间隔 @param bCausal 业务控制身份 @return 无 */
    static void Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
    {
        FBBBMaodieTestNetworkProcessor::Update(Actor, DeltaSeconds, bCausal);
    }
};
