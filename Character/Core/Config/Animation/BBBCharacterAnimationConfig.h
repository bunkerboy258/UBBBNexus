#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterAnimationConfig.generated.h"

/**
 * 配置动画系统从角色实际运动中识别表现事实所需的阈值
 */
USTRUCT(BlueprintType)
struct FBBBCharacterAnimationConfig
{
    GENERATED_BODY()

    /** 角色实际水平转速超过该值时生成对应方向的原地转身信号，单位为度每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Facing", meta = (ClampMin = "0.0"))
    float TurnSignalRateThreshold = 20.0f;

    /** 将角色离散旋转量转换为稳定动画转速所使用的低通时间，单位为秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Animation|Facing", meta = (ClampMin = "0.001"))
    float TurnRateSmoothingTime = 0.12f;
};
