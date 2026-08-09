#pragma once

#include "CoreMinimal.h"
#include "BBBAimConfig.generated.h"

/** 瞄准系统配置 */
USTRUCT(BlueprintType)
struct FBBBAimConfig
{
    GENERATED_BODY()

    /** 瞄准目标沿控制器视线投射的距离 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0"))
    float AimTargetDistance = 10000.0f;

    /** 移动瞄准时身体追随控制器朝向的速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0"))
    float MovingFacingInterpSpeed = 18.0f;

    /** 低于该水平速度时进入原地转身处理 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0"))
    float StationarySpeedThreshold = 5.0f;

    /** 静止瞄准偏角超过该值时开始原地转身 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float TurnInPlaceStartAngle = 40.0f;

    /** 原地转身偏角回落到该值时停止转身 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float TurnInPlaceStopAngle = 10.0f;

    /** 静止瞄准时身体追赶控制器朝向的速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0"))
    float TurnInPlaceInterpSpeed = 8.0f;
};

/** 瞄准动画表现配置 */
USTRUCT(BlueprintType)
struct FBBBAimAnimationConfig
{
    GENERATED_BODY()

    /** 计算瞄准IK起点时使用的角色骨骼 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AimIKOriginBoneName = FName("spine_03");

    /** 动画水平瞄准偏角的插值速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimOffsetYawInterpSpeed = 15.0f;

    /** 是否平滑瞄准IK目标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableAimIKTargetSmoothing = true;

    /** 瞄准IK目标平滑的收敛时间 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIKTargetSmoothTime = 0.1f;

    /** 是否按近距离衰减瞄准IK权重 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableNearAimIKDistanceAlpha = true;

    /** 瞄准IK权重完全衰减的目标距离 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MinAimIKTargetDistance = 60.0f;

    /** 瞄准IK权重完全恢复的目标距离 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float SafeAimIKTargetDistance = 120.0f;

    /** 瞄准IK总权重的插值速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIKAlphaInterpSpeed = 8.0f;
};
