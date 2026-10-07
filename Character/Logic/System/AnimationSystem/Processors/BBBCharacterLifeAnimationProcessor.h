#pragma once

struct FBBBCharacterAnimationUpdateContext;

/** 生命阶段改变时清理动作并提交倒地入场动画 */
class FBBBCharacterLifeAnimationProcessor final
{
  public:
    /**
     * @param Context	本帧动画依赖
     * @return 无
     */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
