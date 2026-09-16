#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentReloadConfig.generated.h"

class UAnimMontage;

/** 换弹时长与人物表现的静态配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentReloadConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Reload", meta = (ClampMin = "0.01"))
    float DurationSeconds = 2.2f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Reload")
    TObjectPtr<UAnimMontage> Montage = nullptr;
};
