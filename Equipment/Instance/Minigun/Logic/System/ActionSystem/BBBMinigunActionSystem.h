#pragma once

struct FBBBMinigunUpdateContext;

/** 转管机枪Action系统的固定调度入口 */
class FBBBMinigunActionSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBMinigunUpdateContext &Context);
};
