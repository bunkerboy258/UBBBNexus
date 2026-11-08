#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterTraversalSettings.generated.h"

/** 上台与翻墙的逻辑高度和时长 不依赖可视身体存在 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterTraversalSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "允许攀爬"))
    bool bEnabled = true;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "快速上台最高高度", Units = "cm", ClampMin = "36", ClampMax = "150"))
    float MantleHeight = 120.0f;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "攀爬最高高度", Units = "cm", ClampMin = "120", ClampMax = "300"))
    float MaximumHeight = 250.0f;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "入口接近距离", Units = "cm", ClampMin = "5", ClampMax = "40"))
    float EntryTolerance = 20.0f;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "失败重试间隔", Units = "s", ClampMin = "0.5", ClampMax = "10"))
    float RetrySeconds = 2.0f;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "快速上台时长", Units = "s", ClampMin = "0.5", ClampMax = "4"))
    float MantleSeconds = 1.5f;

    UPROPERTY(EditAnywhere, Category = "攀爬", meta = (DisplayName = "较高攀爬时长", Units = "s", ClampMin = "1", ClampMax = "6"))
    float ClimbSeconds = 3.1f;

    /** @return 高度与时长是否在可用范围 */
    bool IsValid() const
    {
        return FMath::IsFinite(MantleHeight) && MantleHeight > 35 && MantleHeight <= 150
            && FMath::IsFinite(MaximumHeight) && MaximumHeight >= MantleHeight && MaximumHeight <= 300
            && FMath::IsFinite(EntryTolerance) && EntryTolerance >= 5 && EntryTolerance <= 40
            && FMath::IsFinite(RetrySeconds) && RetrySeconds >= 0.5f && RetrySeconds <= 10
            && FMath::IsFinite(MantleSeconds) && MantleSeconds >= 0.5f && MantleSeconds <= 4
            && FMath::IsFinite(ClimbSeconds) && ClimbSeconds >= 1 && ClimbSeconds <= 6;
    }
};
