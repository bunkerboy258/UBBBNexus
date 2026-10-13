#pragma once

struct FBBBPistolUpdateContext;

/** 手枪只读事实的发送处理器 */
class FBBBPistolNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBPistolUpdateContext &Context);
};
