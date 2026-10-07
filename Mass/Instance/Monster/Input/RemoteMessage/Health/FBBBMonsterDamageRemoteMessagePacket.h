#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageContribution.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"

struct FBBBMonsterHealthInputFragment;

/** 来自远端的累计贡献快照 接收顺序不能减少任何来源的贡献 */
struct FBBBMonsterDamageRemoteMessagePacket final
{
    using FInputFragment = FBBBMonsterHealthInputFragment;

    FGuid InstanceId;
    TArray<FBBBMonsterDamageContribution> Contributions;

    bool IsValid() const
    {
        return InstanceId.IsValid() && !Contributions.ContainsByPredicate(
            [](const auto& Value)
            {
                return !Value.IsValid();
            });
    }

    bool CanApply(const FBBBMonsterNetworkFragment& State) const
    {
        return InstanceId == State.InstanceId;
    }

    /** 整理当前结果包 不保存逐次消息 */
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
