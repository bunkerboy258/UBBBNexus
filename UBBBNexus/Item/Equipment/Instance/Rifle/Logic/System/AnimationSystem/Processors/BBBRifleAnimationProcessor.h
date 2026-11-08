#pragma once

struct FBBBRifleAnimationUpdateContext;

/** 发布动作快照并驱动本次实际需要的动画 */
class FBBBRifleAnimationProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBRifleAnimationUpdateContext &Context);

    /** @param Context	卸下前的更新上下文 @return 无 */
    static void Stop(FBBBRifleAnimationUpdateContext &Context);
};
