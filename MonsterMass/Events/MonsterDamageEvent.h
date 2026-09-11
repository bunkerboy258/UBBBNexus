#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "MonsterDamageEvent.generated.h"

/** 等待逻辑层消费的小怪受伤事件 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterDamageEventFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 等待本帧消费的累计伤害 */
    float PendingDamage = 0.0f;
};
