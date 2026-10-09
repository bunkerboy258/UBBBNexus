#pragma once

struct FBBBShotgunUpdateContext;

/** 在游戏线程计算当前握持目标 */
class FBBBShotgunPoseProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBShotgunUpdateContext &Context);
};
