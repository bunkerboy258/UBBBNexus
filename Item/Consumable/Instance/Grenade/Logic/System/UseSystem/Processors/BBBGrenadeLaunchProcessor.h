#pragma once
struct FBBBGrenadeUseUpdateContext;
/** 手雷Launch处理器 */
class FBBBGrenadeLaunchProcessor final
{
public:
    /** @param Context	本次领域上下文 @return 无 */
    void Update(FBBBGrenadeUseUpdateContext &Context) const;
};
