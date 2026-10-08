#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "BBBCharacterAnimationConfig.generated.h"

class UAnimInstance;
/**
 * 配置动画系统从角色实际运动中识别表现事实所需的阈值
 */
USTRUCT(BlueprintType)
struct FBBBCharacterAnimationConfig
{
    GENERATED_BODY()

    /** 未装备专用动画层时链接的默认动画层 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|动画层", meta = (DisplayName = "默认动画层类"))
    TSubclassOf<UAnimInstance> DefaultAnimationLayerClass;

    /** 角色实际水平转速超过该值时生成对应方向的原地转身信号 单位为度每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|朝向", meta = (ClampMin = "0.0", DisplayName = "转身信号转速阈值"))
    float TurnSignalRateThreshold = 20.0f;

    /** 将角色离散旋转量转换为稳定动画转速所使用的低通时间 单位为秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|朝向", meta = (ClampMin = "0.001", DisplayName = "转速平滑时间"))
    float TurnRateSmoothingTime = 0.12f;

    /** 动画转速每秒允许变化的最大幅度 单位为度每平方秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|朝向", meta = (ClampMin = "0.0", DisplayName = "最大转速变化速度"))
    float MaxTurnRateChangeSpeed = 720.0f;

    /** 额外瞄准上下与左右偏移的最大绝对角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|动画|冲量", meta = (DisplayName = "瞄准冲量角度上限"))
    FVector2D AimImpulseLimitDegrees = FVector2D(30.0f, 30.0f);

};
