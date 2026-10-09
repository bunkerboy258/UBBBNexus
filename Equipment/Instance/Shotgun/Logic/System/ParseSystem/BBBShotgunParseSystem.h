#pragma once

struct FBBBShotgunUpdateContext;

/** 霰弹枪Parse系统的固定调度入口 */
class FBBBShotgunParseSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBShotgunUpdateContext &Context);
};
