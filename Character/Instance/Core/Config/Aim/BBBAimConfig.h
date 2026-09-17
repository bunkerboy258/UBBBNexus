#pragma once

#include "CoreMinimal.h"
#include "BBBAimConfig.generated.h"

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

};
