#pragma once

struct FBBBMeleeUpdateContext;

/** 近战只读事实的发送处理器 */
class FBBBMeleeNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBMeleeUpdateContext &Context);
};
