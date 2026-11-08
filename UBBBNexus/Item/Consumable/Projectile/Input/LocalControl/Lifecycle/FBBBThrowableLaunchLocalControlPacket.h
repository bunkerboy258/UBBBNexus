#pragma once
#include "CoreMinimal.h"
class UBBBThrowableDefinition;
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/DomainData/States/BBBThrowableLifetimeState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/BBBThrowableDefinition.h"

/** 已成立的投掷释放参数 */
struct FBBBThrowableLaunchLocalControlPacket final
{
    UBBBThrowableDefinition* Definition = nullptr;
    FTransform Transform = FTransform::Identity;
    FVector InheritedVelocity = FVector::ZeroVector;
    /** @return 输入值是否有效 */
    bool IsValid() const
    {
        return Definition && Definition->IsValid() && !Transform.ContainsNaN() && !InheritedVelocity.ContainsNaN();
    }
    /** @param State	生命周期状态 @return 是否允许初次释放 */
    bool CanApply(const FBBBThrowableLifetimeState& State) const
    {
        return !State.bLaunched && !State.PendingDefinition;
    }
    /** @param State	生命周期状态 @return 无 */
    void Apply(FBBBThrowableLifetimeState& State) const
    {
        State.PendingDefinition = Definition;
        State.PendingTransform = Transform;
        State.PendingVelocity = InheritedVelocity;
    }
};
