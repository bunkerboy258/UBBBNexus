#pragma once
struct FBBBHealthPackParseUpdateContext;
/** Parse阶段处理器 */
class FBBBHealthPackParseProcessor final
{
  public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBHealthPackParseUpdateContext &Context) const;
};
