#pragma once

struct FBBBRevolverUpdateContext;

/** 左轮网络身份的固定发送调度 */
class FBBBRevolverNetworkSystem final
{
public:
    /** @param Context	本帧结果 @param bAuthority	主管线确认的主机身份 @return 无 */
    static void Update(FBBBRevolverUpdateContext &Context, bool bAuthority);
};
