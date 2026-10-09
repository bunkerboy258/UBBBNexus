#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/NetworkSystem/DomainData/States/BBBSMGNetworkObservationState.h"

/** 冲锋枪网络领域状态的唯一持有者 */
struct FBBBSMGNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBSMGNetworkObservationState &ReadSMGNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBSMGNetworkProcessor;
    FBBBSMGNetworkObservationState ObservationState;
};
