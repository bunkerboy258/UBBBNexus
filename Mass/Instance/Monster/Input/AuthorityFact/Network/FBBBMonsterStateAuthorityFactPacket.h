#pragma once

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"

struct FBBBMonsterNetworkInputFragment;

/** 小怪当前行动与生命事实 不重演远端决策 */
struct FBBBMonsterStateAuthorityFactPacket final
{
    using FInputFragment = FBBBMonsterNetworkInputFragment;

    FGuid InstanceId;
    uint32 Revision = 0;
    FTransform Transform = FTransform::Identity;
    FVector Velocity = FVector::ZeroVector;
    EBBBMonsterBehavior Behavior = EBBBMonsterBehavior::Idle;
    uint32 ActionId = 0;
    float StateEnteredTime = 0.0f;
    float Health = 0.0f;

    /** @return 当前事实是否有效 */
    bool IsValid() const
    {
        return InstanceId.IsValid() && Revision > 0
            && !Transform.ContainsNaN() && !Velocity.ContainsNaN()
            && FMath::IsFinite(Health) && Health >= 0.0f
            && FMath::IsFinite(StateEnteredTime)
            && Behavior <= EBBBMonsterBehavior::Dead;
    }

    /** @return 身份与版本是否允许还原 */
    bool CanApply(const FBBBMonsterNetworkFragment& Network) const
    {
        return (!Network.InstanceId.IsValid() || InstanceId == Network.InstanceId) && Revision > Network.ReceivedRevision;
    }

    /**
     * @param Network	接收版本
     * @param Position	位置结果
     * @param Speed	速度结果
     * @param State	行为状态
     * @param Damage	生命结果
     * @return 无
     */
    void Apply(FBBBMonsterNetworkFragment& Network, FTransformFragment& Position,
        FMassVelocityFragment& Speed, FBBBMonsterBehaviorFragment& State,
        FBBBMonsterDamageFragment& Damage) const
    {
        Network.InstanceId = InstanceId;
        Network.ReceivedRevision = Revision;
        Damage.ReportedHealth = Health;
        if (State.State == EBBBMonsterBehavior::Dead)
        {
            return;
        }

        Position.GetMutableTransform() = Transform;
        Speed.Value = Velocity;
        State.State = Behavior;
        State.ActionId = ActionId;
        State.StateEnteredTime = StateEnteredTime;
    }
};
