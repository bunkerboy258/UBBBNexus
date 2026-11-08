#pragma once
struct FBBBCharacterAnimationUpdateContext;
/** 从当前物理姿态选片并执行已经批准的起身动作 */
class FBBBCharacterGetUpAnimationProcessor final
{
public:
    /** @param Context 动画更新依赖 @return 无 */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
