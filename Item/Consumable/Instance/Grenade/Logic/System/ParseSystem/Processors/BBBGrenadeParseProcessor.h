#pragma once
struct FBBBGrenadeParseUpdateContext;
/** 手雷Parse处理器 */
class FBBBGrenadeParseProcessor final
{
public:
    /** @param Context	本次领域上下文 @return 无 */
    void Update(FBBBGrenadeParseUpdateContext &Context) const;
};
