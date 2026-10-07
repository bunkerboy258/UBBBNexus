#pragma once

struct FBBBRifleUpdateContext;

/** 步枪只读事实的发送处理器 */
class FBBBRifleNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);
};
