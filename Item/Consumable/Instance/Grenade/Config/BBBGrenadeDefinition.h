#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBGrenadeDefinition.generated.h"
class UBBBProjectileDefinition;

/** 手雷的投掷配置 不持有角色动画引用 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBGrenadeDefinition final : public UBBBConsumableDefinition
{
    GENERATED_BODY()
public:
    UBBBGrenadeDefinition()
    {
        MaxStack = 4;
    }

    /** 投出后使用的独立实体配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|手雷", meta = (DisplayName = "飞行实体"))
    TObjectPtr<UBBBProjectileDefinition> Projectile = nullptr;

    /** 相对视线的固定抬手角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|手雷", meta = (DisplayName = "投掷抬角", ClampMin = "0", ClampMax = "45", ForceUnits = "deg"))
    float PitchOffset = 12.0f;
};
