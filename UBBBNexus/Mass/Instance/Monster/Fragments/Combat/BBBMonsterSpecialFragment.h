#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterSpecialFact.h"
#include "BBBMonsterSpecialFragment.generated.h"

/** 特感独享的当前动作 冷却与本轮接触集合 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterSpecialFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 网络和动画读取的当前动作 */
    FBBBMonsterSpecialFact Fact;
    /** 下一次允许技能开始的世界时间 */
    float ReadyAt = 0;
    /** 下一次潜行候选检查时间 */
    float NextStalkCheck = 0;
    /** 分摊候选预算时轮换两侧方向 防止只检查第一组位置 */
    uint8 StalkCandidateOffset = 0;
    /** 本次冲撞已经推进的长度 */
    float RushTravelled = 0;
    /** 同次冲撞只影响每个玩家一次 */
    TWeakObjectPtr<AActor> HitPlayers[32];
    /** 本轮接触的有效数量 */
    uint8 HitPlayerCount = 0;
};

/** 弱引用使用引擎复制语义 不持有每实体堆分配 */
template<>
struct TMassFragmentTraits<FBBBMonsterSpecialFragment>
{
    enum { AuthorAcceptsItsNotTriviallyCopyable = true };
};
