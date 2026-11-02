#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageTotals.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"

struct FBBBMonsterHealthInputFragment;

/** 同一目标的独立命中 输入阶段不计算伤害结果 */
struct FBBBMonsterDamageLocalControlPacket final
{
    using FInputFragment = FBBBMonsterHealthInputFragment;

    /** 按进入目标的顺序保留全部独立命中 */
    TArray<FBBBHitContribution> Hits;

    bool IsValid() const
    {
        return !Hits.IsEmpty() && !Hits.ContainsByPredicate([](const auto& Hit)
        {
            return !Hit.IsValid() || Hit.Part > 5 || (!Hit.bWorldSource && Hit.PlayerId < 0);
        });
    }

    bool CanApply(const FBBBMonsterDamageFragment&) const
    {
        return true;
    }

    void Apply(FBBBMonsterDamageFragment& State) const
    {
        State.PendingHits.Append(Hits);
    }
};
