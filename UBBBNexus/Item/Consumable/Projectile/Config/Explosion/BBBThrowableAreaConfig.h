#pragma once
#include "CoreMinimal.h"
#include "BBBThrowableAreaConfig.generated.h"

/** 起爆后独立持续的地面区域 */
USTRUCT(BlueprintType)
struct FBBBThrowableAreaConfig final
{
    GENERATED_BODY()

    /** 生效时长 零为关闭 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "区域时长", ClampMin = "0", ForceUnits = "s"))
    float Duration = 0.0f;

    /** 地面覆盖半径 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "区域半径", ClampMin = "0", ForceUnits = "cm"))
    float Radius = 0.0f;

    /** 首次生效时覆盖的最大半径比例 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "初始区域半径比例", ClampMin = "0", ClampMax = "1"))
    float InitialRadiusRatio = 1.0f;

    /** 展开到最大半径所需时间 零为立即展开 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "区域展开时间", ClampMin = "0", ForceUnits = "s"))
    float ExpansionSeconds = 0.0f;

    /** 自落点往上的有效高度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "区域高度", ClampMin = "0", ForceUnits = "cm"))
    float Height = 0.0f;

    /** 持续效果是否切断怪物视觉采样 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "遮挡怪物视线"))
    bool bObscuresSight = false;

    /** 火区每次结算的伤害 零为无伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "周期伤害", ClampMin = "0"))
    float Damage = 0.0f;

    /** 相邻两次区域伤害的最短间隔 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "伤害间隔", ClampMin = "0.2", ForceUnits = "s"))
    float DamageInterval = 1.0f;

    /** @param Age	起爆后经过的服务器时间 @return 当前生效半径 */
    float RadiusAtAge(const double Age) const
    {
        if (ExpansionSeconds <= 0.0f)
        {
            return Radius;
        }
        const float Alpha = static_cast<float>(FMath::Clamp(Age / ExpansionSeconds, 0.0, 1.0));
        return Radius * FMath::Lerp(InitialRadiusRatio, 1.0f, FMath::SmoothStep(0.0f, 1.0f, Alpha));
    }
};
