#pragma once

struct FBBBCharacterAnimationUpdateContext;

/** 更新额外瞄准冲击并修正动画目标 */
class FBBBCharacterAimImpulseProcessor final
{
public:
    /**
     * 消费冲击并更新角色动画黑板
     * @param Context	角色动画上下文
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
