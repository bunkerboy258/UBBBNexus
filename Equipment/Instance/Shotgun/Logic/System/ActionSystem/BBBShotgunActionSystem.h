#pragma once

struct FBBBShotgunUpdateContext;

/** 霰弹枪Action系统的固定调度入口 */
class FBBBShotgunActionSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBShotgunUpdateContext &Context);
};
