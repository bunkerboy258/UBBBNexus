#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/NetworkSystem/DomainData/States/BBBMinigunNetworkObservationState.h"

/** 转管机枪网络领域状态的唯一持有者 */
struct FBBBMinigunNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBMinigunNetworkObservationState &ReadMinigunNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBMinigunNetworkProcessor;
    FBBBMinigunNetworkObservationState ObservationState;
};
