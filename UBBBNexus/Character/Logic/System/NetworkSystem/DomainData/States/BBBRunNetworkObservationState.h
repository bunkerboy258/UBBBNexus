#pragma once

#include "CoreMinimal.h"
#include "BBBRunNetworkObservationState.generated.h"

/** 跑步网络观察处理器需要跨帧保留的状态 */
USTRUCT()
struct FBBBRunNetworkObservationState final
{
    GENERATED_BODY()

    /** 最近一次已经上传的跑步状态 */
    TOptional<bool> LastObservedRun;
};
