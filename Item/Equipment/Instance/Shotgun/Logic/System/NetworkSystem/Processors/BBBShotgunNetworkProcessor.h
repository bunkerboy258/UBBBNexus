#pragma once

struct FBBBShotgunNetworkUpdateContext;

/** 霰弹枪只读事实的发送处理器 */
class FBBBShotgunNetworkProcessor final
{
public:
    /** @param Context	本帧已生成的结果 @return 无 */
    static void Update(FBBBShotgunNetworkUpdateContext &Context);
};
