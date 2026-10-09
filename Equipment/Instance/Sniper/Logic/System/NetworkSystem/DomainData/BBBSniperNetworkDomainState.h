#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/NetworkSystem/DomainData/States/BBBSniperNetworkObservationState.h"

/** 狙击枪网络领域状态的唯一持有者 */
struct FBBBSniperNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBSniperNetworkObservationState &ReadSniperNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBSniperNetworkProcessor;
    FBBBSniperNetworkObservationState ObservationState;
};
