#pragma once
#include "CoreMinimal.h"

/** 已发送的生命结果版本 */
struct FBBBCharacterLifeNetworkObservationState final
{
    /** 生命结果的发送基准 */
    uint64 Revision = 0;
};
