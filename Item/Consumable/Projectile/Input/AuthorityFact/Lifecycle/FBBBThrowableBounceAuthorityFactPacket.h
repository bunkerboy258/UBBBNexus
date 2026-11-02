#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/DomainData/States/BBBThrowableLifetimeState.h"

/** 远端只接收碰撞声音结果 不重新产生物理碰撞 */
struct FBBBThrowableBounceAuthorityFactPacket final
{
    TArray<FBBBThrowableBounceFact> Facts;
    /** @return 碰撞结果是否有效 */
    bool IsValid() const
    {
        return !Facts.IsEmpty() && !Facts.ContainsByPredicate([](const FBBBThrowableBounceFact& Fact)
        {
            return Fact.Serial == 0 || Fact.Position.ContainsNaN() || !FMath::IsFinite(Fact.Strength) || Fact.Strength < 60.0f;
        });
    }
    /** @param State	生命周期状态 @return 是否为更新的结果 */
    bool CanApply(const FBBBThrowableLifetimeState& State) const
    {
        return Facts.ContainsByPredicate([&State](const FBBBThrowableBounceFact& Fact)
        {
            return Fact.Serial > State.Bounce.Serial;
        });
    }
    /** @param State	生命周期状态 @return 无 */
    void Apply(FBBBThrowableLifetimeState& State) const
    {
        for (const auto& Fact : Facts)
        {
            if (Fact.Serial > State.Bounce.Serial)
            {
                State.Bounce = Fact;
            }
        }
    }
};
