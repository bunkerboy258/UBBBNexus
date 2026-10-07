#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/DomainData/States/BBBRifleNetworkObservationState.h"

/** 步枪网络领域状态的唯一持有者 */
struct FBBBRifleNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBRifleNetworkObservationState &ReadRifleNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBRifleNetworkProcessor;
    FBBBRifleNetworkObservationState ObservationState;
};
