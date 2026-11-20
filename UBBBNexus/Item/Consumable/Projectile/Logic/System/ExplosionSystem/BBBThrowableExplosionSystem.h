#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/Processors/BBBThrowableExplosionProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/Processors/BBBThrowableAreaProcessor.h"
/** Explosion领域固定调度入口 */
class FBBBThrowableExplosionSystem final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Update(ABBBThrowable& Actor) const
    {
        FBBBThrowableExplosionProcessor{}.Update(Actor);
        FBBBThrowableAreaProcessor{}.Update(Actor);
    }
};
