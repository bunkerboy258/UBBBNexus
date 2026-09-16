#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentActionPresentation.generated.h"

class UAnimMontage;

/** 单次装备动作的本地人物表现数据 */
USTRUCT(BlueprintType)
struct FBBBEquipmentActionPresentation
{
    GENERATED_BODY()

    /** 人物动作蒙太奇 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** 人物动作蒙太奇播放倍率 */
    UPROPERTY(BlueprintReadOnly)
    float PlayRate = 1.0f;
};
