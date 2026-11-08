#pragma once

struct FBBBSMGAnimationUpdateContext;

/** 发布动作快照并驱动本次实际需要的动画 */
class FBBBSMGAnimationProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBSMGAnimationUpdateContext &Context);

    /** @param Context	卸下前的更新上下文 @return 无 */
    static void Stop(FBBBSMGAnimationUpdateContext &Context);
};
