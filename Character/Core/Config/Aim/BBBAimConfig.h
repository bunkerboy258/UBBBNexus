
#pragma once
#include "CoreMinimal.h"
#include "BBBAimConfig.generated.h"

/**
 * 瞄准核心配置
 */
USTRUCT(BlueprintType)
struct FBBBAimConfig
{
    GENERATED_BODY()

    /** 屏幕中心射线的最大检测距离  */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimTraceDistance = 10000.0f;

};

/**
 * 瞄准表现配置
 */
USTRUCT(BlueprintType)
struct FBBBAimAnimationConfig
{
    GENERATED_BODY()

    /** 是否对瞄准目标点做阻尼弹簧平滑 关闭时直接使用原始目标点 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableAimIKTargetSmoothing = true;

    /** IK目标点阻尼弹簧平滑的收敛时间 秒 小于等于0时平滑不生效 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIKTargetSmoothTime = 0.1f;

    /** 是否按距离衰减IK权重 关闭时距离权重恒为1 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableNearAimIKDistanceAlpha = true;

    /** 近距离IK衰减下限 目标距离不超过该值时IK权重为0 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MinAimIKTargetDistance = 60.0f;

    /** 近距离IK衰减上限 目标距离达到该值时IK权重恢复为1 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float SafeAimIKTargetDistance = 120.0f;
    /** 整个瞄准 IK 权重的淡入淡出速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIKAlphaInterpSpeed = 8.0f;
};
