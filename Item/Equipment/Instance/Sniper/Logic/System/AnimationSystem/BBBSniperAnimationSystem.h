#pragma once

struct FBBBSniperUpdateContext;

/** 狙击枪Animation系统的固定调度入口 */
class FBBBSniperAnimationSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBSniperUpdateContext &Context);
};
