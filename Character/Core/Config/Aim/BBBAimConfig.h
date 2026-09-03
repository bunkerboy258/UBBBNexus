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

};

/** 瞄准动画表现配置 */
USTRUCT(BlueprintType)
struct FBBBAimAnimationConfig
{
    GENERATED_BODY()

    /** 计算瞄准IK起点时使用的角色骨骼 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AimIKOriginBoneName = FName("spine_03");

    /** 是否平滑瞄准IK目标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnableAimIKTargetSmoothing = true;

    /** 瞄准IK目标平滑的收敛时间 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIKTargetSmoothTime = 0.1f;

    /** 瞄准意图强度的插值速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIntentAlphaInterpSpeed = 8.0f;

    /** 瞄准IK锁值的插值速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimIKLockAlphaInterpSpeed = 8.0f;
};
