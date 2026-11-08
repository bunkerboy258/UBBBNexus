#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterSpecialAnimationPresentation.generated.h"

/** 特殊动作的采样与混合参数 不改变技能判定 */
USTRUCT(BlueprintType)
struct FBBBMonsterSpecialAnimationPresentation final
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (DisplayName = "回到普通动作", Units = "s", ClampMin = "0.01", ClampMax = "0.5"))
    float NormalBlend = .18f;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "进入蓄力", Units = "s", ClampMin = "0.01", ClampMax = "0.5"))
    float WindupBlend = .18f;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "进入冲撞", Units = "s", ClampMin = "0.01", ClampMax = "0.5"))
    float RushBlend = .12f;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "进入收势", Units = "s", ClampMin = "0.01", ClampMax = "0.5"))
    float RecoveryBlend = .22f;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "进入喷吐", Units = "s", ClampMin = "0.01", ClampMax = "0.5"))
    float SpitBlend = .16f;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "冲撞每循环前进距离", Units = "cm", ClampMin = "10", ClampMax = "1200"))
    float RushStride = 320;

    /** @return 时长与步幅是否有限且有界 */
    bool IsValid() const
    {
        for (float Value : {NormalBlend, WindupBlend, RushBlend, RecoveryBlend, SpitBlend})
        {
            if (!FMath::IsFinite(Value) || Value < .01f || Value > .5f)
            {
                return false;
            }
        }
        return FMath::IsFinite(RushStride) && RushStride >= 10 && RushStride <= 1200;
    }
};
