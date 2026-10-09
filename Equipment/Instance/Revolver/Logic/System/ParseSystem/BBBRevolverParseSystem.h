#pragma once

struct FBBBRevolverUpdateContext;

/** 左轮Parse系统的固定调度入口 */
class FBBBRevolverParseSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBRevolverUpdateContext &Context);
};
