#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/NetworkSystem/DomainData/States/BBBPistolNetworkObservationState.h"

/** 手枪网络领域状态的唯一持有者 */
struct FBBBPistolNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBPistolNetworkObservationState &ReadPistolNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBPistolNetworkProcessor;
    FBBBPistolNetworkObservationState ObservationState;
};
