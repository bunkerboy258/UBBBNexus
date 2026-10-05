#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterHealthFragment.generated.h"

/** 小怪生命值状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterHealthFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "当前生命值"))
    float CurrentHealth = 100.0f;

    /** 最大生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "最大生命值"))
    float MaxHealth = 100.0f;

    /** 每次受伤重新计算的硬直时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (ClampMin = "0.01", DisplayName = "受击持续时间"))
    float HurtDuration = 0.2f;

    /** 死亡表现保留到实体回收的时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (ClampMin = "0.01", DisplayName = "死亡后存活时间"))
    float DeathLifetime = 3.0f;
};
