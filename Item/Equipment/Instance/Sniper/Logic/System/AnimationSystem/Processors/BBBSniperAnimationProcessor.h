#pragma once

struct FBBBSniperAnimationUpdateContext;

/** 发布动作快照并驱动本次实际需要的动画 */
class FBBBSniperAnimationProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBSniperAnimationUpdateContext &Context);

    /** @param Context	卸下前的更新上下文 @return 无 */
    static void Stop(FBBBSniperAnimationUpdateContext &Context);
};
