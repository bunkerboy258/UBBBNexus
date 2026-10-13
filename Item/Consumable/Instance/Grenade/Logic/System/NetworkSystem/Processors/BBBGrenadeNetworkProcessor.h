#pragma once
struct FBBBGrenadeNetworkUpdateContext;
/** 手雷Network处理器 */
class FBBBGrenadeNetworkProcessor final
{
public:
    /** @param Context	本次领域上下文 @return 无 */
    void Update(FBBBGrenadeNetworkUpdateContext &Context) const;
};
