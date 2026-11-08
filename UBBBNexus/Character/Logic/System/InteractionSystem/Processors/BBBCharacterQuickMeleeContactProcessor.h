#pragma once
struct FBBBCharacterInteractionUpdateContext;
/** 仅在角色快速近战通知窗口内采样接触 */
class FBBBCharacterQuickMeleeContactProcessor final
{
  public:
    /** @param Context 更新依赖 @return 无 */
    void Update(FBBBCharacterInteractionUpdateContext &Context) const;
};
