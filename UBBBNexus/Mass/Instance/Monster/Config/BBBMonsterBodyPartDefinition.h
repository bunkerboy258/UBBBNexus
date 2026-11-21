#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterBodyPartDefinition.generated.h"

/** 单个部位的生命与伤害传导静态规则 */
USTRUCT(BlueprintType)
struct FBBBMonsterBodyPartDefinition final
{
    GENERATED_BODY()

    /** 相对整体生命上限的部位生命比例 */
    UPROPERTY(EditAnywhere, Category = "部位", meta = (ClampMin = "0.01", DisplayName = "部位生命比例"))
    float HealthFraction = 1.0f;

    /** 有效伤害中使用耐久伤害的比例 */
    UPROPERTY(EditAnywhere, Category = "部位", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "耐久比例"))
    float Durability = 0.25f;

    /** 对整体生命的有效伤害传导比例 */
    UPROPERTY(EditAnywhere, Category = "部位", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "整体伤害传导"))
    float MainTransfer = 1.0f;

    /** 部位生命耗尽即致死 */
    UPROPERTY(EditAnywhere, Category = "部位", meta = (DisplayName = "致死部位"))
    bool bFatal = true;

    /** 范围贡献的部位分配权重 */
    UPROPERTY(EditAnywhere, Category = "部位", meta = (ClampMin = "0", DisplayName = "范围伤害权重"))
    float RadialWeight = 0.1f;

    /** 本部位对踉跄强度的修正倍率 */
    UPROPERTY(EditAnywhere, Category = "部位", meta = (ClampMin = "0.0", ClampMax = "4.0", DisplayName = "踉跄强度倍率"))
    float StaggerMultiplier = 1.0f;

    /** 本部位完整原地反应的原片秒数 */
    UPROPERTY(EditAnywhere, Category = "部位", meta = (ClampMin = "0.1", ClampMax = "10.0", Units = "s", DisplayName = "完整踉跄时长"))
    float StaggerDuration = 2.3333333f;

    /**
     * @param Damage	普通伤害
     * @param DurableDamage	耐久伤害
     * @return 按本部位耐久比例计算的有效伤害
     */
    double ResolveDamage(const double Damage, const double DurableDamage) const
    {
        return Damage * (1.0 - Durability) + DurableDamage * Durability;
    }

    /**
     * @param MaxHealth	小怪最大生命值
     * @return 本部位损毁所需的累计有效伤害
     */
    double GetHealthThreshold(const double MaxHealth) const
    {
        return MaxHealth * HealthFraction;
    }

    /**
     * @param Damage	部位累计有效伤害
     * @param MaxHealth	小怪最大生命值
     * @return 部位生命是否耗尽
     */
    bool HasReachedHealthThreshold(const double Damage, const double MaxHealth) const
    {
        return Damage + UE_SMALL_NUMBER >= GetHealthThreshold(MaxHealth);
    }

    /** @return 静态规则是否有效 */
    bool IsValid() const
    {
        return FMath::IsFinite(RadialWeight) && RadialWeight >= 0 && FMath::IsFinite(HealthFraction) && HealthFraction > 0.0f
            && FMath::IsFinite(Durability) && Durability >= 0.0f && Durability <= 1.0f
            && FMath::IsFinite(MainTransfer) && MainTransfer >= 0.0f && MainTransfer <= 1.0f
            && FMath::IsFinite(StaggerMultiplier) && StaggerMultiplier >= 0.0f && StaggerMultiplier <= 4.0f
            && FMath::IsFinite(StaggerDuration) && StaggerDuration >= 0.1f && StaggerDuration <= 10.0f;
    }
};
