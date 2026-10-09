#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/NetworkSystem/DomainData/States/BBBShotgunNetworkObservationState.h"

/** 霰弹枪网络领域状态的唯一持有者 */
struct FBBBShotgunNetworkDomainState final
{
    /** @return 当前事实发送基准 */
    const FBBBShotgunNetworkObservationState &ReadShotgunNetworkObservationState() const
    {
        return ObservationState;
    }

private:
    friend class FBBBShotgunNetworkProcessor;
    FBBBShotgunNetworkObservationState ObservationState;
};
