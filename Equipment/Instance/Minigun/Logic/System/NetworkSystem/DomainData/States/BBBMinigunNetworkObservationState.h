#pragma once

#include "CoreMinimal.h"

/** 转管机枪已经发送的当前事实基准 */
struct FBBBMinigunNetworkObservationState final
{
    /** 已发送的持有实例 */
    uint64 Generation = 0;

    /** 已发布本次持有的基准 */
    bool bPublished = false;

    /** 已发送的FireSequence事实 */
    int32 FireSequence = 0;

    /** 已发送的ReloadSequence事实 */
    int32 ReloadSequence = 0;

    /** 已发送的bReloading事实 */
    bool bReloading = false;

private:
    friend struct FBBBMinigunNetworkDomainState;
    FBBBMinigunNetworkObservationState() = default;
};
