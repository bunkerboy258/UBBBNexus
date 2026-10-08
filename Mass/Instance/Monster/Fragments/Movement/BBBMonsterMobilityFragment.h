#pragma once

#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterMobilityFragment.generated.h"

/** 伤害处理器维护的当前受击减速与持续爬行结果 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterMobilityFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前减速开始时的最低速度比例 */
    float SlowMinimumRatio = 1.0f;

    /** 当前减速开始时间 使用本机世界秒数 */
    float SlowRecoveryStartedAt = 0.0f;

    /** 当前减速恢复完成时间 */
    float SlowEndsAt = 0.0f;

    /** 当前水平停顿的结束时间 不暂停重力 */
    float HitStopEndsAt = 0.0f;

    /** 当前踉跄的开始时间 不保存命中历史 */
    float StaggerStartedAt = 0.0f;

    /** 当前踉跄结束时间 连射不重新开始本段动作 */
    float StaggerEndsAt = 0.0f;

    /** 当前踉跄使用的首个命中部位 后续命中不切换姿势 */
    EBBBMonsterHitRegion StaggerRegion = EBBBMonsterHitRegion::Torso;

    /**
     * @param Now	本机世界秒数
     * @return 当前是否处于站立踉跄
     */
    bool IsStaggering(const float Now) const
    {
        return !bCrawling && StaggerEndsAt > StaggerStartedAt && Now >= StaggerStartedAt && Now < StaggerEndsAt;
    }

    /** 腿伤达到阈值后保持到本代实体结束 */
    bool bCrawling = false;

    /** 首次转入爬行的本机世界时间 */
    float CrawlStartedAt = 0.0f;

    /** @return 当前减速比例 保持阶段不提前回升 */
    float GetSpeedRatio(const float Now) const
    {
        if (Now < HitStopEndsAt)
        {
            return 0.0f;
        }
        const float Progress = FMath::Clamp((Now - SlowRecoveryStartedAt)
            / FMath::Max(SlowEndsAt - SlowRecoveryStartedAt, SMALL_NUMBER), 0.0f, 1.0f);
        return FMath::Lerp(SlowMinimumRatio, 1.0f, Progress);
    }
};
