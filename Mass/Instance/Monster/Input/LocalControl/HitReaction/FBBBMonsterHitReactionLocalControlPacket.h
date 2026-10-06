#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"

struct FBBBMonsterHitReactionInputFragment;

/** 本机或镜像弹丸成立的最近命中表现 */
struct FBBBMonsterHitReactionLocalControlPacket final
{
    using FInputFragment = FBBBMonsterHitReactionInputFragment;

    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Torso;
    FVector Position = FVector::ZeroVector;
    FVector Direction = FVector::ForwardVector;
    FVector Normal = FVector::UpVector;

    /** @return 命中数据是否有限且方向有效 */
    bool IsValid() const
    {
        return static_cast<uint8>(Region) <= static_cast<uint8>(EBBBMonsterHitRegion::RightLeg)
            && !Position.ContainsNaN() && !Direction.ContainsNaN() && !Normal.ContainsNaN()
            && !Direction.IsNearlyZero() && !Normal.IsNearlyZero();
    }

    /**
     * @param Health	目标当前生命
     * @return 死亡目标不接收迟到表现
     */
    bool CanApply(const FBBBMonsterHealthFragment& Health) const
    {
        return Health.CurrentHealth > 0.0f;
    }

    /**
     * @param State	目标最近命中事实
     * @return 无返回值
     */
    void Apply(FBBBMonsterHitReactionFragment& State) const
    {
        State.Region = Region;
        State.Position = Position;
        State.Direction = Direction.GetSafeNormal();
        State.Normal = Normal.GetSafeNormal();
        State.Age = 0.0f;
        ++State.Serial;
    }
};
