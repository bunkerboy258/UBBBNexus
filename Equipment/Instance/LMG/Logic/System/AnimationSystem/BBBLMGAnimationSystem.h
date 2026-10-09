#pragma once

struct FBBBLMGUpdateContext;

/** 轻机枪Animation系统的固定调度入口 */
class FBBBLMGAnimationSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBLMGUpdateContext &Context);
};
