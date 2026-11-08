#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterAnimationUpdateContext;

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
    /**
     * 播放已经批准的离散动画动作
     * @param Context 本次动画更新上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
