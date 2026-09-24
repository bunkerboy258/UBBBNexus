#pragma once

struct FBBBRifleUpdateContext;

/** 步枪Network系统的固定调度入口 */
class FBBBRifleNetworkSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);
};
