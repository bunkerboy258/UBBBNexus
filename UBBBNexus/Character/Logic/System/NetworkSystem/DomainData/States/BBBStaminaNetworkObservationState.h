#pragma once

#include "CoreMinimal.h"

/** 已发送的体力结果版本 */
struct FBBBStaminaNetworkObservationState final
{
    /** 体力结果的发送基准 */
    uint64 Revision = 0;
};
