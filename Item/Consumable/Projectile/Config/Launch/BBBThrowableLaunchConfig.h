#pragma once
#include "CoreMinimal.h"
#include "BBBThrowableLaunchConfig.generated.h"

/** 投掷物Launch静态配置 */
USTRUCT(BlueprintType)
struct FBBBThrowableLaunchConfig final
{
    GENERATED_BODY()

    /** 沿释放方向的初速度 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "初速度", ToolTip = "沿释放方向的初速度 单位厘米每秒", ClampMin = "1", ForceUnits = "cm/s"))
    float Speed = 1400.0f;

    /** 本地轴角速度 单位度每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "初始角速度", ToolTip = "本地轴角速度 单位度每秒"))
    FVector AngularVelocity = FVector(0.0, 360.0, 0.0);

    /** 投掷时叠加的持有者速度倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "持有者速度继承", ToolTip = "投掷时叠加的持有者速度倍率", ClampMin = "0"))
    float VelocityInheritance = 0.5f;
};
