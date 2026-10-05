#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterDamageContribution.generated.h"

/** 一个玩家对一代小怪已经造成的累计伤害 不是单次命中事件 */
USTRUCT()
struct FBBBMonsterDamageContribution final
{
    GENERATED_BODY()

    /** 同一局内由 PlayerState 提供的稳定玩家身份 */
    UPROPERTY()
    int32 PlayerId = INDEX_NONE;

    /** 累计有效伤害 只增不减 */
    UPROPERTY()
    double Damage = 0.0;

    bool IsValid() const
    {
        return PlayerId >= 0 && FMath::IsFinite(Damage) && Damage >= 0.0;
    }

    bool operator==(const FBBBMonsterDamageContribution& Other) const
    {
        return PlayerId == Other.PlayerId && Damage == Other.Damage;
    }
};
