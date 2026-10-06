#pragma once
struct FBBBCharacterAnimationUpdateContext;

/** 只把翻越事实派发给已配置的链接动画层 */
class FBBBCharacterTraversalAnimationProcessor final
{
public:
    /** @param Context 动画更新上下文 @return 无 */
    void Update(FBBBCharacterAnimationUpdateContext &Context) const;
};
