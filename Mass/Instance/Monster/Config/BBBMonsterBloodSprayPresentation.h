#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterBloodSprayPresentation.generated.h"

/** 单次喷血通过共享通道传递的有界倍率 不增加粒子数量 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterBloodSprayPresentation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta=(DisplayName="尺寸倍率", ClampMin="0.05", ClampMax="3"))
    float Size = 1;

    UPROPERTY(EditAnywhere, meta=(DisplayName="速度倍率", ClampMin="0", ClampMax="3"))
    float Speed = 1;

    UPROPERTY(EditAnywhere, meta=(DisplayName="持续时间倍率", ClampMin="0.05", ClampMax="2"))
    float Lifetime = 1;

    UPROPERTY(EditAnywhere, meta=(DisplayName="不透明度倍率", ClampMin="0", ClampMax="1"))
    float Opacity = 1;

    /** @return 编辑器外写入同样遵守预算和有限值约束 */
    bool IsValid() const
    {
        return FMath::IsFinite(Size) && Size >= 0.05f && Size <= 3
            && FMath::IsFinite(Speed) && Speed >= 0 && Speed <= 3
            && FMath::IsFinite(Lifetime) && Lifetime >= 0.05f && Lifetime <= 2
            && FMath::IsFinite(Opacity) && Opacity >= 0 && Opacity <= 1;
    }
};
