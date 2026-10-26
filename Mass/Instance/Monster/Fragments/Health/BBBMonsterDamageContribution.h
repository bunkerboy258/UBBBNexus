#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterPartDamage.h"
#include "BBBMonsterStaggerContribution.h"
#include "BBBMonsterDestructionContribution.h"
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
    FBBBMonsterPartDamage Parts;

    /** 同一有效命中成立的八区毁伤贡献 */
    UPROPERTY()
    FBBBMonsterDestructionContribution Destruction;

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

    /** 子弹来源已成立的当前踉跄结果 镜像不重新抽概率 */
    UPROPERTY()
    FBBBMonsterStaggerContribution Stagger;

    bool IsValid() const
    {
        return PlayerId >= 0 && Parts.IsValid()
            && FMath::IsFinite(LastHitTime) && LastHitRegion <= EBBBMonsterHitRegion::RightLeg
            && !LastSourcePosition.ContainsNaN() && Stagger.IsValid();
    }

    bool operator==(const FBBBMonsterDamageContribution& Other) const
    {
        return PlayerId == Other.PlayerId && Parts == Other.Parts
            && LastHitTime == Other.LastHitTime && LastHitRegion == Other.LastHitRegion
            && LastSourcePosition == Other.LastSourcePosition && bHasSourcePosition == Other.bHasSourcePosition
            && Stagger == Other.Stagger && Destruction == Other.Destruction;
    }

    /**
     * @param Other	同一玩家的当前结果
     * @return 无
     */
    void Merge(const FBBBMonsterDamageContribution& Other)
    {
        FBBBMonsterPartDamage Combined = Parts;
        Combined.MergeMax(Other.Parts);
        FBBBMonsterStaggerContribution CombinedStagger = Stagger;
        CombinedStagger.Merge(Other.Stagger);
        FBBBMonsterDestructionContribution CombinedDestruction = Destruction;
        CombinedDestruction.MergeMax(Other.Destruction);
        if (PlayerId == INDEX_NONE || Other.LastHitTime > LastHitTime
            || (Other.LastHitTime == LastHitTime && (Other.LastHitRegion > LastHitRegion
                || (Other.LastHitRegion == LastHitRegion && Other.Parts.Exceeds(Parts)))))
        {
            *this = Other;
        }
        Parts = Combined;
        Stagger = CombinedStagger;
        Destruction = CombinedDestruction;
    }
};
