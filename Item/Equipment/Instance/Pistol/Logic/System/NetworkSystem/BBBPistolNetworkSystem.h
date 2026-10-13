#pragma once

struct FBBBPistolUpdateContext;

/** 手枪网络身份的固定发送调度 */
class FBBBPistolNetworkSystem final
{
public:
    /** @param Context	本帧结果 @param bAuthority	主管线确认的主机身份 @return 无 */
    static void Update(FBBBPistolUpdateContext &Context, bool bAuthority);
};
