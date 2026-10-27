#pragma once

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Presentation/BBBMonsterGoreFragment.h"

struct FBBBMonsterNetworkInputFragment;

/** 小怪当前行动事实 不重演远端决策 生命由本机伤害字典决定 */
struct FBBBMonsterStateAuthorityFactPacket final
{
    using FInputFragment = FBBBMonsterNetworkInputFragment;

    FGuid InstanceId;
    /** 出生种子只允许在本机首次初始化前还原 */
    uint32 BirthSeed = 0;
    uint32 Revision = 0;
    /** 只还原当前伤口资源身份 不确认生命损毁 */
    uint8 DismembermentPresets[5] = {};
    FTransform Transform = FTransform::Identity;
    FVector Velocity = FVector::ZeroVector;
    EBBBMonsterBehavior Behavior = EBBBMonsterBehavior::Idle;
    uint32 ActionId = 0;
    float StateEnteredTime = 0.0f;
    /** 已转换到本机时间的当前爬行进度起点 */
    float CrawlStartedAt = -1.0f;
    /** 已有爬行时刻 本机刚加入时起点可以早于零 */
    bool bHasCrawlTime = false;

    /** @return 当前事实是否有效 */
    bool IsValid() const
    {
        return InstanceId.IsValid() && Revision > 0
            && !Transform.ContainsNaN() && !Velocity.ContainsNaN()
            && FMath::IsFinite(StateEnteredTime)
            && FMath::IsFinite(CrawlStartedAt)
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
     * @param Gore	当前断点外观结果
     * @return 无
     */
    void Apply(FBBBMonsterNetworkFragment& Network, FTransformFragment& Position,
        FMassVelocityFragment& Speed, FBBBMonsterBehaviorFragment& State, FBBBMonsterGoreFragment& Gore) const
    {
        Network.InstanceId = InstanceId;
        Network.ReceivedRevision = Revision;
        Network.CrawlStartedAt = CrawlStartedAt;
        Network.bHasCrawlTime = bHasCrawlTime;
        if (Behavior == EBBBMonsterBehavior::Dead)
        {
            Network.DeathStartedAt = StateEnteredTime;
            Network.bHasDeathTime = true;
        }
        for (int32 Index = 0; Index < 5; ++Index)
        {
            Gore.DismembermentPresets[Index] = DismembermentPresets[Index];
        }
        if (State.State == EBBBMonsterBehavior::Dead)
        {
            if (Behavior == EBBBMonsterBehavior::Dead)
            {
                Position.GetMutableTransform() = Transform;
                Speed.Value = Velocity;
            }
            return;
        }

        Position.GetMutableTransform() = Transform;
        Speed.Value = Velocity;
        // 远端 Dead 不是本机死亡依据 生命 Processor 会根据累计贡献独立进入死亡
        if (Behavior == EBBBMonsterBehavior::Dead)
        {
            Speed.Value = FVector::ZeroVector;
            return;
        }
        State.State = Behavior;
        State.ActionId = ActionId;
        State.StateEnteredTime = StateEnteredTime;
    }
};
