#pragma once
struct FBBBGrenadeParseUpdateContext;
/** Parse阶段处理器 */
class FBBBGrenadeParseProcessor final
{
  public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBGrenadeParseUpdateContext &Context) const;
};
