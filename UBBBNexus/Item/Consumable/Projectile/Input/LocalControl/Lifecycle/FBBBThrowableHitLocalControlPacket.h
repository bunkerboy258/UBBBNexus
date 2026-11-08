#pragma once

#include "BBBWork/UBBBNexus/Combat/FBBBHitContribution.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/DomainData/States/BBBThrowableLifetimeState.h"

/** 世界投掷物受到的独立战斗作用 */
struct FBBBThrowableHitLocalControlPacket final
{
    /** 保留本轮各次完整贡献 */
    TArray<FBBBHitContribution> Hits;

    /** @return 是否包含完整有效的命中贡献 */
    bool IsValid() const
    {
        return !Hits.IsEmpty() && !Hits.ContainsByPredicate([](const auto& Hit)
        {
            return !Hit.IsValid();
        });
    }

    /** @param State 目标生命周期 @return 是否已经释放 */
    bool CanApply(const FBBBThrowableLifetimeState& State) const
    {
        return State.bLaunched;
    }

    /** @param State 目标待处理输入 @return 无 */
    void Apply(FBBBThrowableLifetimeState& State) const
    {
        State.Hits.Append(Hits);
    }
};
