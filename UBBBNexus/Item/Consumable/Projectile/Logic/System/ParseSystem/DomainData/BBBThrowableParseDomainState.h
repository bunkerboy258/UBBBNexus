#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ParseSystem/DomainData/States/BBBThrowableInputState.h"
/** Parse领域唯一状态持有者 */
struct FBBBThrowableParseDomainState final
{
    /** @return 本领域只读状态 */
    const FBBBThrowableInputState& ReadInputState() const { return State; }
private:
    friend class FBBBThrowableParseProcessor;
    friend class FBBBThrowableLifetimeProcessor;
    friend class FBBBThrowableExplosionProcessor;
    friend class FBBBThrowableNetworkProcessor;
    friend class FBBBThrowableShutdown;
    FBBBThrowableInputState State;
};
