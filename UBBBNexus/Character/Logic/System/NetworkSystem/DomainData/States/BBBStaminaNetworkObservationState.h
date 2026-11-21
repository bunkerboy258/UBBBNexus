#pragma once

#include "CoreMinimal.h"
#include "BBBStaminaNetworkObservationState.generated.h"

/** 已发送的体力结果版本 */
USTRUCT()
struct FBBBStaminaNetworkObservationState final
{
    GENERATED_BODY()

    /** 体力结果的发送基准 */
    uint64 Revision = 0;
};
