#pragma once
struct FBBBCharacterInteractionUpdateContext;
/** 角色当次消耗品Release阶段 */
class FBBBCharacterUseReleaseProcessor final
{
  public:
    /** @param Context 本次角色更新依赖 @return 无 */
    void Update(FBBBCharacterInteractionUpdateContext &Context) const;
};
