#pragma once

struct FBBBMinigunUpdateContext;

/** 在游戏线程计算当前握持目标 */
class FBBBMinigunPoseProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBMinigunUpdateContext &Context);
};
