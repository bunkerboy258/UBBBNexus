#pragma once

struct FBBBMinigunUpdateContext;

/** 转管机枪只读事实的发送处理器 */
class FBBBMinigunNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBMinigunUpdateContext &Context);
};
