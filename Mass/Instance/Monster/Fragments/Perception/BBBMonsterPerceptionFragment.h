#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterPerceptionFragment.generated.h"

/** 小怪侦察配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterPerceptionFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 能够发现玩家的最大距离 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
    float SightRange = 3500.0f;
};
