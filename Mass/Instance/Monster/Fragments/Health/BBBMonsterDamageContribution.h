#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
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

    /** 本玩家对腿部造成的累计有效伤害 */
    UPROPERTY()
    double LegDamage = 0.0;

    /** 最近有效命中的服务器世界时间 不保存中间命中 */
    UPROPERTY()
    double LastHitTime = -10.0;

    /** 最近有效命中的部位 */
    UPROPERTY()
    EBBBMonsterHitRegion LastHitRegion = EBBBMonsterHitRegion::Torso;

    /** 最近有效命中成立时的来源位置 不更新为射手后续位置 */
    UPROPERTY()
    FVector LastSourcePosition = FVector::ZeroVector;

    /** 最近命中是否提供有效来源位置 */
    UPROPERTY()
    bool bHasSourcePosition = false;

    bool IsValid() const
    {
        return PlayerId >= 0 && FMath::IsFinite(Damage) && Damage >= 0.0
            && FMath::IsFinite(LegDamage) && LegDamage >= 0.0 && LegDamage <= Damage
            && FMath::IsFinite(LastHitTime) && LastHitRegion <= EBBBMonsterHitRegion::RightLeg
            && !LastSourcePosition.ContainsNaN();
    }

    bool operator==(const FBBBMonsterDamageContribution& Other) const
    {
        return PlayerId == Other.PlayerId && Damage == Other.Damage && LegDamage == Other.LegDamage
            && LastHitTime == Other.LastHitTime && LastHitRegion == Other.LastHitRegion
            && LastSourcePosition == Other.LastSourcePosition && bHasSourcePosition == Other.bHasSourcePosition;
    }
};
