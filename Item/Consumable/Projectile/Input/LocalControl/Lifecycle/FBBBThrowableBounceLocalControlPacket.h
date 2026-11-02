#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/DomainData/States/BBBThrowableLifetimeState.h"

/** 物理碰撞回调贡献真实接触 不直接执行表现 */
struct FBBBThrowableBounceLocalControlPacket final
{
    TArray<FBBBThrowableBounceFact> Contacts;
    /** @return 接触数据是否有效 */
    bool IsValid() const
    {
        return !Contacts.IsEmpty() && !Contacts.ContainsByPredicate([](const FBBBThrowableBounceFact& Contact)
        {
            return Contact.Position.ContainsNaN() || !FMath::IsFinite(Contact.Strength) || Contact.Strength < 0.0f;
        });
    }
    /** @param State	生命周期状态 @return 是否已经释放 */
    bool CanApply(const FBBBThrowableLifetimeState& State) const
    {
        return State.bLaunched;
    }
    /** @param State	生命周期状态 @return 无 */
    void Apply(FBBBThrowableLifetimeState& State) const
    {
        for (const auto& Contact : Contacts)
        {
            if (Contact.Strength > State.PendingBounceStrength)
            {
                State.PendingBounceStrength = Contact.Strength;
                State.PendingBouncePosition = Contact.Position;
            }
        }
    }
};
