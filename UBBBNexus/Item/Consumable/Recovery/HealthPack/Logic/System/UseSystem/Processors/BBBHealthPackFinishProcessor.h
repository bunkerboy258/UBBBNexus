#pragma once
struct FBBBHealthPackUseUpdateContext;
/** Finish阶段处理器 */
class FBBBHealthPackFinishProcessor final
{
  public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBHealthPackUseUpdateContext &Context) const;
};
