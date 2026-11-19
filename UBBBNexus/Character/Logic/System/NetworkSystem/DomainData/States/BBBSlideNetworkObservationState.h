#pragma once

#include "CoreMinimal.h"
#include "BBBSlideNetworkObservationState.generated.h"

/** 滑铲网络观察处理器需要跨帧保留的状态 */
USTRUCT()
struct FBBBSlideNetworkObservationState final
{
    GENERATED_BODY()

    /** 最近一次已经上传的滑铲状态 */
    TOptional<bool> LastObservedSlide;
};
