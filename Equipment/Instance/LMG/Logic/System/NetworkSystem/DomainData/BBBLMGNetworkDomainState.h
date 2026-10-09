#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/NetworkSystem/DomainData/States/BBBLMGNetworkObservationState.h"

/** 轻机枪网络领域状态的唯一持有者 */
struct FBBBLMGNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBLMGNetworkObservationState &ReadLMGNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBLMGNetworkProcessor;
    FBBBLMGNetworkObservationState ObservationState;
};
