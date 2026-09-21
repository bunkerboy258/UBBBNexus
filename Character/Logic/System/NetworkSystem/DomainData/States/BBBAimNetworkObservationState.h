#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBReplicatedAimState.h"
#include "BBBAimNetworkObservationState.generated.h"

/** 瞄准网络观察处理器需要跨帧保留的状态 */
USTRUCT()
struct FBBBAimNetworkObservationState final
{
    GENERATED_BODY()

    /** 最近一次已经上传的瞄准状态 */
    TOptional<FBBBReplicatedAimState> LastObservedAim;

    /** 最近一次上传瞄准状态的世界时间 */
    float LastAimUploadTime = -1000.0f;
};
