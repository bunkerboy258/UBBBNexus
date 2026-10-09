#pragma once

struct FBBBSniperUpdateContext;

/** 狙击枪只读事实的发送处理器 */
class FBBBSniperNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBSniperUpdateContext &Context);
};
