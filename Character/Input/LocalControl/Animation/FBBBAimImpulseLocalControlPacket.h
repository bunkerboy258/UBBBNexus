#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAimImpulseState.h"

/** 提交角色动画瞄准方向的瞬时角度冲击 */
struct FBBBAimImpulseLocalControlPacket final
{
    /** 向上与向右为正 单位为度 */
    FVector2D ImpulseDegrees = FVector2D::ZeroVector;

    /** 瞄准时使用的上下与左右冲量 */
    FVector2D AimingImpulseDegrees = FVector2D::ZeroVector;

    /** 腾空时乘到方向冲量的倍率 */
    float AirborneScale = 1.0f;

    /** 腰射冲击回零速度 */
    float RecoverySpeed = 14.0f;

    /** 瞄准冲击回零速度 */
    float AimingRecoverySpeed = 14.0f;

    /** @return 输入角度是否有限 */
    bool IsValid() const
    {
        return !ImpulseDegrees.ContainsNaN() && !AimingImpulseDegrees.ContainsNaN()
            && FMath::IsFinite(AirborneScale) && AirborneScale >= 0.0f
            && FMath::IsFinite(RecoverySpeed) && RecoverySpeed > 0.0f
            && FMath::IsFinite(AimingRecoverySpeed) && AimingRecoverySpeed > 0.0f;
    }

    /**
     * 判断冲击是否可进入动画黑板
     * @param Context	角色输入上下文
     * @return 是否允许应用
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /**
     * 累加待消费的角度冲击
     * @param Context	角色输入上下文
     * @return 无
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        FVector2D Impulse = ImpulseDegrees;
        Context.AimImpulse.RecoverySpeed = RecoverySpeed;
        if (Context.Control.bAim)
        {
            Impulse = AimingImpulseDegrees;
            Context.AimImpulse.RecoverySpeed = AimingRecoverySpeed;
        }

        if (Context.AnimationFacts.MovementMode == MOVE_Falling)
        {
            Impulse *= AirborneScale;
        }

        Context.AimImpulse.PendingDegrees += Impulse;
    }
};
