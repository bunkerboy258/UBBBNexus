#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/DomainData/States/BBBGrenadeNetworkState.h"

/** 手雷Network领域的唯一持有者 */
struct FBBBGrenadeNetworkDomainState final
{
public:
    /** @return NetworkState 的只读结果 */
    const FBBBGrenadeNetworkState &ReadGrenadeNetworkState() const
    {
        return NetworkState;
    }
private:
    friend class FBBBGrenadeNetworkProcessor;
    friend class FBBBGrenadeInitializer;
    FBBBGrenadeNetworkState NetworkState;
};
