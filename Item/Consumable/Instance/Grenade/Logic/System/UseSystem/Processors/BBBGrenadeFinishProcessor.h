#pragma once
struct FBBBGrenadeUseUpdateContext;
/** 手雷Finish处理器 */
class FBBBGrenadeFinishProcessor final
{
public:
    /** @param Context	本次领域上下文 @return 无 */
    void Update(FBBBGrenadeUseUpdateContext &Context) const;
};
