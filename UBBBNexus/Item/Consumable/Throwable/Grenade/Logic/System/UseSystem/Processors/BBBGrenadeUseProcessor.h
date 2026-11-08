#pragma once
struct FBBBGrenadeUseUpdateContext;
/** Use阶段处理器 */
class FBBBGrenadeUseProcessor final
{
  public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBGrenadeUseUpdateContext &Context) const;
};
