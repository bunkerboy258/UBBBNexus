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
};
