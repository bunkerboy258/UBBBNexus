#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAimImpulseState.h"

/** 提交角色动画瞄准方向的瞬时角度冲击 */
struct FBBBAimImpulseLocalControlPacket final
{
    /** 向上与向右为正 单位为度 */
    FVector2D ImpulseDegrees = FVector2D::ZeroVector;

    /** @return 输入角度是否有限 */
    bool IsValid() const
    {
        return !ImpulseDegrees.ContainsNaN();
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
        Context.AimImpulse.PendingDegrees += ImpulseDegrees;
    }
};
