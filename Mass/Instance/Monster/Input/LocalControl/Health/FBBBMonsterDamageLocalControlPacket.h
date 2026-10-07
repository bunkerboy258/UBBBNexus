#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageContribution.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"

struct FBBBMonsterHealthInputFragment;

/** 本机累计贡献快照 后来的快照包含前面已经成立的命中 */
struct FBBBMonsterDamageLocalControlPacket final
{
    using FInputFragment = FBBBMonsterHealthInputFragment;

    TArray<FBBBMonsterDamageContribution> Contributions;

    bool IsValid() const
    {
        return !Contributions.IsEmpty() && !Contributions.ContainsByPredicate(
            [](const auto& Value)
            {
                return !Value.IsValid();
            });
    }

    bool CanApply(const FBBBMonsterDamageFragment& State) const
    {
        return Contributions.ContainsByPredicate([&State](const auto& Value)
        {
            const auto* Current = State.Contributions.Find(Value.PlayerId);
            return Current == nullptr || Value.Damage > Current->Damage || Value.LegDamage > Current->LegDamage;
        });
    }

    /** 整理当前结果包 不保存逐次命中 */
    void Include(const FBBBMonsterDamageContribution& Value)
    {
        auto* Existing = Contributions.FindByPredicate(
            [&Value](const auto& Item)
            {
                return Item.PlayerId == Value.PlayerId;
            });
        if (Existing != nullptr)
        {
            const double LegDamage = FMath::Max(Existing->LegDamage, Value.LegDamage);
            if (Value.Damage > Existing->Damage)
            {
                *Existing = Value;
            }
            Existing->LegDamage = LegDamage;
        }
        else
        {
            Contributions.Add(Value);
        }
    }

    void Apply(FBBBMonsterDamageFragment& State) const
    {
        for (const auto& Value : Contributions)
        {
            auto& Current = State.Contributions.FindOrAdd(Value.PlayerId);
            const double LegDamage = FMath::Max(Current.LegDamage, Value.LegDamage);
            if (Current.PlayerId == INDEX_NONE || Value.Damage > Current.Damage)
            {
                Current = Value;
            }
            Current.LegDamage = LegDamage;
        }
    }
};
