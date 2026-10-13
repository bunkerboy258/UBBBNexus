#pragma once

struct FBBBSniperUpdateContext;

/** 狙击枪网络身份的固定发送调度 */
class FBBBSniperNetworkSystem final
{
public:
    /** @param Context	本帧结果 @param bAuthority	主管线确认的主机身份 @return 无 */
    static void Update(FBBBSniperUpdateContext &Context, bool bAuthority);
};
