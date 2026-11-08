#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/DomainData/States/BBBThrowableExplosionState.h"
/** Explosion领域唯一状态持有者 */
struct FBBBThrowableExplosionDomainState final
{
    /** @return 本领域只读状态 */
    const FBBBThrowableExplosionState& ReadExplosionState() const { return State; }
private:
    friend class FBBBThrowableParseProcessor;
    friend class FBBBThrowableLifetimeProcessor;
    friend class FBBBThrowableExplosionProcessor;
    friend class FBBBThrowableNetworkProcessor;
    friend class FBBBThrowableShutdown;
    FBBBThrowableExplosionState State;
};
