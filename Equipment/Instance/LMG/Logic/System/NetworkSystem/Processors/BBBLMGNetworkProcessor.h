#pragma once

struct FBBBLMGUpdateContext;

/** 轻机枪只读事实的发送处理器 */
class FBBBLMGNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBLMGUpdateContext &Context);
};
