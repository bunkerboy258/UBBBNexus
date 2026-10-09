#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/NetworkSystem/DomainData/States/BBBRevolverNetworkObservationState.h"

/** 左轮网络领域状态的唯一持有者 */
struct FBBBRevolverNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBRevolverNetworkObservationState &ReadRevolverNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBRevolverNetworkProcessor;
    FBBBRevolverNetworkObservationState ObservationState;
};
