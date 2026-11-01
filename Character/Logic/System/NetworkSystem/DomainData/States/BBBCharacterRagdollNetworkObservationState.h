#pragma once
#include "CoreMinimal.h"
/** 当前布娃娃结果的发送基准 */
struct FBBBCharacterRagdollNetworkObservationState final
{
    /** 最近投送的结果版本 */
    uint64 Revision = 0;
    /** 最近投送时间 */
    double LastSendTime = -1.0;
};
