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
    /** 攀爬及交权确认期间暂不接受普通移动纠正 */
    bool bMovementCorrectionSuppressed = false;
    /** 已交还控制但首个普通移动帧尚未获得确认 */
    bool bAwaitingExitMoveAck = false;
    /** 交权后的移动确认必须越过此时间戳 */
    float ExitMoveTimeStamp = 0.0f;
};
