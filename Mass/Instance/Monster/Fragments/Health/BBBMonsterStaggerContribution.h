#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterPartDamage.h"
#include "BBBMonsterStaggerContribution.generated.h"

/** 单个玩家的最新开火候选与当前有效踉跄结果 不保存命中历史 */
USTRUCT()
struct FBBBMonsterStaggerContribution final
{
    GENERATED_BODY()

    static constexpr double RecoveryDuration = 0.35;

    /** 本机来源的最新开火身份 同发霰弹共用 不同步候选缓存 */
    FGuid AttackId;

    /** 本机来源的单调开火排序时间 不是消息接收时间 */
    double AttackTime = 0.0;

    /** 该攻击首次有效命中的服务器时间 */
    double HitTime = 0.0;

    /** 当前攻击最大部位冲击贡献 */
    float Impact = 0.0f;

    /** 当前攻击各部位的累计冲击贡献 同发弹丸先合并后选择部位 */
    FBBBMonsterPartDamage Impacts;

    /** 当前候选的最终概率 */
    float Chance = 0.0f;

    /** 每次攻击对一个目标只取一次的随机样本 */
    float Roll = 0.0f;

    /** 当前候选的完整原片时长 */
    float Duration = 0.0f;

    /** 最大冲击贡献所属部位 */
    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Torso;

    /** 同次攻击后续弹丸不再次触发 */
    bool bEvaluated = false;

    /**
     * @param AvailableAt	当前动作与恢复保护的结束时间
     * @return 本次候选是否首次成立
     */
    bool Resolve(const double AvailableAt)
    {
        if (!AttackId.IsValid() || bEvaluated)
        {
            return false;
        }
        bEvaluated = true;
        return Chance > Roll && HitTime >= AvailableAt && Duration > 0.0f;
    }

    /** 最近已成立的踉跄身份 不因后续未触发命中丢失 */
    UPROPERTY()
    FGuid ActiveAttackId;

    /** 当前踉跄起止时间 使用服务器时间 */
    UPROPERTY()
    double StartedAt = 0.0;

    UPROPERTY()
    double EndsAt = 0.0;

    /** 当前完整动作的锁定部位 */
    UPROPERTY()
    EBBBMonsterHitRegion ActiveRegion = EBBBMonsterHitRegion::Torso;

    /** @return 当前快照是否为有限合法值 */
    bool IsValid() const
    {
        return Impacts.IsValid() && FMath::IsFinite(AttackTime) && AttackTime >= 0.0
            && FMath::IsFinite(HitTime) && HitTime >= 0.0
            && FMath::IsFinite(Impact) && Impact >= 0.0f
            && FMath::IsFinite(Chance) && Chance >= 0.0f && Chance <= 1.0f
            && FMath::IsFinite(Roll) && Roll >= 0.0f && Roll < 1.0f
            && FMath::IsFinite(Duration) && Duration >= 0.0f && Duration <= 10.0f
            && FMath::IsFinite(StartedAt) && StartedAt >= 0.0
            && FMath::IsFinite(EndsAt) && EndsAt >= StartedAt && EndsAt <= StartedAt + 10.0
            && Region <= EBBBMonsterHitRegion::RightLeg && ActiveRegion <= EBBBMonsterHitRegion::RightLeg
            && (!ActiveAttackId.IsValid() || EndsAt > StartedAt);
    }

    bool operator==(const FBBBMonsterStaggerContribution& Other) const
    {
        return AttackId == Other.AttackId && AttackTime == Other.AttackTime && HitTime == Other.HitTime
            && Impact == Other.Impact && Impacts == Other.Impacts && Chance == Other.Chance && Roll == Other.Roll
            && Duration == Other.Duration && Region == Other.Region && bEvaluated == Other.bEvaluated
            && ActiveAttackId == Other.ActiveAttackId && StartedAt == Other.StartedAt
            && EndsAt == Other.EndsAt && ActiveRegion == Other.ActiveRegion;
    }

    /**
     * @param Other	同一玩家的当前结果
     * @return 无
     */
    void Merge(const FBBBMonsterStaggerContribution& Other)
    {
        const FGuid SavedId = ActiveAttackId;
        const double SavedStart = StartedAt;
        const double SavedEnd = EndsAt;
        const auto SavedRegion = ActiveRegion;
        if (Other.AttackId.IsValid() && (!AttackId.IsValid() || Other.AttackTime > AttackTime
            || (Other.AttackTime == AttackTime && AttackId < Other.AttackId)
            || (Other.AttackId == AttackId && (Other.Impact > Impact
                || (Other.Impact == Impact && Other.Region > Region)))))
        {
            *this = Other;
        }
        if (Other.AttackId == AttackId)
        {
            bEvaluated |= Other.bEvaluated;
        }
        ActiveAttackId = SavedId;
        StartedAt = SavedStart;
        EndsAt = SavedEnd;
        ActiveRegion = SavedRegion;
        if (Other.ActiveAttackId.IsValid() && (!ActiveAttackId.IsValid() || Other.StartedAt > StartedAt
            || (Other.StartedAt == StartedAt && ActiveAttackId < Other.ActiveAttackId)))
        {
            ActiveAttackId = Other.ActiveAttackId;
            StartedAt = Other.StartedAt;
            EndsAt = Other.EndsAt;
            ActiveRegion = Other.ActiveRegion;
        }
    }
};
