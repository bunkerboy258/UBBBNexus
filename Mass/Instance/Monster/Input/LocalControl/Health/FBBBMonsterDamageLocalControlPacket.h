#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"

struct FBBBMonsterHealthInputFragment;

/** 本机合法命中的伤害输入 */
struct FBBBMonsterDamageLocalControlPacket final
{
    using FInputFragment = FBBBMonsterHealthInputFragment;

    /** 本次有效伤害 */
    float Damage = 0.0f;

    /** @return 数值是否合法 */
    bool IsValid() const
    {
        return FMath::IsFinite(Damage) && Damage > 0.0f;
    }

    /** @return 当前是否仍存活 */
    bool CanApply(const FBBBMonsterHealthFragment& Health) const
    {
        return Health.CurrentHealth > 0.0f;
    }

    /**
     * @param State	伤害待消费状态
     * @return 无
     */
    void Apply(FBBBMonsterDamageFragment& State) const
    {
        State.PendingDamage = Damage;
    }
};
