#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/DomainData/States/BBBThrowableLifetimeState.h"
/** Lifetime领域唯一状态持有者 */
struct FBBBThrowableLifetimeDomainState final
{
    /** @return 本领域只读状态 */
    const FBBBThrowableLifetimeState& ReadLifetimeState() const { return State; }
private:
    friend class FBBBThrowableParseProcessor;
    friend class FBBBThrowableLifetimeProcessor;
    friend class FBBBThrowableExplosionProcessor;
    friend class FBBBThrowableNetworkProcessor;
    friend class FBBBThrowableShutdown;
    FBBBThrowableLifetimeState State;
};
