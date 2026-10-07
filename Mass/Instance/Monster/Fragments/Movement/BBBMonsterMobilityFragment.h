#pragma once

#include "MassEntityTypes.h"
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

    /** 腿伤达到阈值后保持到本代实体结束 */
    bool bCrawling = false;

    /** 首次转入爬行的本机世界时间 */
    float CrawlStartedAt = 0.0f;

    /** @return 当前减速比例 保持阶段不提前回升 */
    float GetSpeedRatio(const float Now) const
    {
        const float Progress = FMath::Clamp((Now - SlowRecoveryStartedAt)
            / FMath::Max(SlowEndsAt - SlowRecoveryStartedAt, SMALL_NUMBER), 0.0f, 1.0f);
        return FMath::Lerp(SlowMinimumRatio, 1.0f, Progress);
    }
};
