#pragma once

struct FBBBSMGUpdateContext;

/** 冲锋枪只读事实的发送处理器 */
class FBBBSMGNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBSMGUpdateContext &Context);
};
