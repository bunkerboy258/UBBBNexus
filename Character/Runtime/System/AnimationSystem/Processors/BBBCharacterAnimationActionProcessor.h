#pragma once

#include "CoreMinimal.h"

class UBBBAnimInstance;
struct FBBBAnimationState;

/** 消费角色已经仲裁完成的瞬时动画动作 */
class ABBB_EVAC_API FBBBCharacterAnimationActionProcessor final
{
public:
    /**
     * 逐条发布当前帧的装备动画动作
     * @param AnimInstance		动画实例
     * @param AnimationData	角色动画数据与待播放队列
     * @return 无
     */
    void Update(
        UBBBAnimInstance &AnimInstance,
        FBBBAnimationState &AnimationData) const;
};
