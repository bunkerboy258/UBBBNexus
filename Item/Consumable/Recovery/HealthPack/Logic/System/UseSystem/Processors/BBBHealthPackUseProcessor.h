#pragma once
struct FBBBHealthPackUseUpdateContext;
/** Use阶段处理器 */
class FBBBHealthPackUseProcessor final
{
  public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBHealthPackUseUpdateContext &Context) const;
};
