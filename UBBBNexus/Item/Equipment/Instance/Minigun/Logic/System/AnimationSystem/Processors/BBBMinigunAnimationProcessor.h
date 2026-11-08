#pragma once

struct FBBBMinigunAnimationUpdateContext;

/** 发布动作快照并驱动本次实际需要的动画 */
class FBBBMinigunAnimationProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBMinigunAnimationUpdateContext &Context);

    /** @param Context	卸下前的更新上下文 @return 无 */
    static void Stop(FBBBMinigunAnimationUpdateContext &Context);
};
