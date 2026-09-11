#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "MonsterDeathEvent.generated.h"

/** 小怪死亡后的展示与回收状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterDeathEventFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 死亡表现结束后的回收时间 */
    float DestroyAtTime = -1.0f;
};
