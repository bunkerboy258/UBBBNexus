#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/NetworkSystem/DomainData/States/BBBMeleeNetworkObservationState.h"

/** 近战网络领域状态的唯一持有者 */
struct FBBBMeleeNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBMeleeNetworkObservationState &ReadMeleeNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBMeleeNetworkProcessor;
    FBBBMeleeNetworkObservationState ObservationState;
};
