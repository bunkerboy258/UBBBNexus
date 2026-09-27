#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"

struct FBBBMonsterHealthInputFragment;

/** 远端已经成立的剩余生命结果 */
struct FBBBMonsterHealthRemoteMessagePacket final
{
    using FInputFragment = FBBBMonsterHealthInputFragment;

    /** 远端当前剩余血量 */
    float Health = 0.0f;

    /** @return 是否为有限非负结果 */
    bool IsValid() const
    {
        return FMath::IsFinite(Health) && Health >= 0.0f;
    }

    /** @return 是否能降低当前生命 */
    bool CanApply(const FBBBMonsterHealthFragment& State) const
    {
        return State.CurrentHealth > Health;
    }

    /**
     * @param State	待合并的生命结果
     * @return 无
     */
    void Apply(FBBBMonsterDamageFragment& State) const
    {
        State.ReportedHealth = Health;
    }
};
