#pragma once
struct FBBBGrenadeUseUpdateContext;
/** Finish阶段处理器 */
class FBBBGrenadeFinishProcessor final
{
  public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBGrenadeUseUpdateContext &Context) const;
};
