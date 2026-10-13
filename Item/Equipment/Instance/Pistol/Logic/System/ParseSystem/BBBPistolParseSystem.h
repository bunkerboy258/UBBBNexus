#pragma once

struct FBBBPistolUpdateContext;

/** 手枪Parse系统的固定调度入口 */
class FBBBPistolParseSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBPistolUpdateContext &Context);
};
