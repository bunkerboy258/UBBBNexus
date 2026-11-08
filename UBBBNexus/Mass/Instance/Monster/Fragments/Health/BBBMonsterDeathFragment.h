#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "BBBMonsterDeathFragment.generated.h"

/** 小怪死亡后的展示与回收状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterDeathFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 死亡表现结束后的回收时间 */
    float DestroyAtTime = -1.0f;

    /** 首次死亡时的移动惯性 只用于本地尸体表现 */
    FVector InitialVelocity = FVector::ZeroVector;
};
