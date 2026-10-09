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

    /** @return 静态规则是否有效 */
    bool IsValid() const
    {
        return FMath::IsFinite(HealthFraction) && HealthFraction > 0.0f
            && FMath::IsFinite(Durability) && Durability >= 0.0f && Durability <= 1.0f
            && FMath::IsFinite(MainTransfer) && MainTransfer >= 0.0f && MainTransfer <= 1.0f;
    }
};
