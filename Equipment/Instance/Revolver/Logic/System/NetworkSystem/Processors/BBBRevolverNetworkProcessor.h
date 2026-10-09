#pragma once

struct FBBBRevolverUpdateContext;

/** 左轮只读事实的发送处理器 */
class FBBBRevolverNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBRevolverUpdateContext &Context);
};
