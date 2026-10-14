#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/NetworkSystem/DomainData/States/BBBThrowableNetworkState.h"
/** 网络领域状态唯一持有者 */
struct FBBBThrowableNetworkDomainState final
{
    /** @return 当前网络交接状态 */
    const FBBBThrowableNetworkState& ReadNetworkState() const { return State; }
private:
    friend class FBBBThrowableNetworkProcessor;
    friend class FBBBThrowableShutdown;
    FBBBThrowableNetworkState State;
};
