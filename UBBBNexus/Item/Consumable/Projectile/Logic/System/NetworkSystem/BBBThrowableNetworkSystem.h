#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/NetworkSystem/Processors/BBBThrowableNetworkProcessor.h"
/** Network领域固定调度入口 */
class FBBBThrowableNetworkSystem final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Update(ABBBThrowable& Actor) const { FBBBThrowableNetworkProcessor{}.Update(Actor); }
};
