#pragma once
#include "CoreMinimal.h"
#include "BBBTraversalNetworkObservationState.generated.h"

/** 翻越开始与结束的离散同步基准 */
USTRUCT()
struct FBBBTraversalNetworkObservationState final
{
    GENERATED_BODY()
    /** 上次观察到的动作序号 */
    uint32 LastActionId = 0;
    /** 上次观察到的执行状态 */
    bool bLastActive = false;
};
