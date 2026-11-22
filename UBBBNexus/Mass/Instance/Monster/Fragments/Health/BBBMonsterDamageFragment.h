#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterDamageTotals.h"
#include "BBBWork/UBBBNexus/Combat/FBBBHitContribution.h"
#include "BBBMonsterDamageFragment.generated.h"

/** 已成立的玩家累计贡献与本轮受伤结果 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterDamageFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 键为玩家身份 值为对这一代小怪的累计伤害 */
    UPROPERTY()
    TMap<int64, FBBBMonsterDamageTotals> Contributions;

    /** 已解析且等待本轮生命处理的独立命中 */
    TArray<FBBBHitContribution> PendingHits;

    /** 每个累计写入来源已经消费的响应版本 不保存命中历史 */
    TMap<int64, uint64> PresentedHitSequences;

    /** 已扣血的受伤事实 */
    bool bReceivedDamage = false;

    /** 房主下一次允许结算余燃的服务器时间 零表示无任务 */
    double NextBurnDamageAt = 0.0;
};

/** 累计字典需要原生构造 拷贝与析构 明确接受 Mass 非平凡 Fragment */
template<>
struct TMassFragmentTraits<FBBBMonsterDamageFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
