#pragma once
struct FBBBCharacterInteractionUpdateContext;
/** 只转交直接绑定装备的角色动画输入 */
class FBBBCharacterInteractionInputProcessor final
{
public:
    /** @param Context 装备通信上下文 @return 无 */
    void Update(FBBBCharacterInteractionUpdateContext &Context) const;
};
