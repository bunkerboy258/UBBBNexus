#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/DomainData/States/BBBGrenadeNetworkObservationState.h"

/** 手雷Network领域的唯一持有者 */
struct FBBBGrenadeNetworkDomainState final
{
public:
    /** @return NetworkState 的只读结果 */
    const FBBBGrenadeNetworkObservationState &ReadGrenadeNetworkObservationState() const
    {
        return NetworkObservationState;
    }
private:
    friend class FBBBGrenadeNetworkProcessor;
    FBBBGrenadeNetworkObservationState NetworkObservationState;
};
