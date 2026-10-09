#pragma once

struct FBBBSniperUpdateContext;

/** 狙击枪Parse系统的固定调度入口 */
class FBBBSniperParseSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBSniperUpdateContext &Context);
};
