#pragma once

struct FBBBSMGUpdateContext;

/** 冲锋枪Parse系统的固定调度入口 */
class FBBBSMGParseSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBSMGUpdateContext &Context);
};
