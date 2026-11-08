#pragma once

#include "CoreMinimal.h"
#include "BBBAccelerationNetworkObservationState.generated.h"

/** 加速度快照的上传和分发基准 */
USTRUCT()
struct FBBBAccelerationNetworkObservationState final
{
    GENERATED_BODY()

    /** 控制者最近观察到的实际加速度 */
    FVector LastObservedAcceleration = FVector::ZeroVector;

    /** 控制者最近观察到的已解析移动输入 */
    FVector LastObservedMovementInput = FVector::ZeroVector;

    /** 当前控制者生成或主机已经分发的结果版本 */
    uint64 Revision = 0;

    /** 最近上传的世界时间用于限制连续快照频率 */
    double LastUploadTime = -1.0;
};
