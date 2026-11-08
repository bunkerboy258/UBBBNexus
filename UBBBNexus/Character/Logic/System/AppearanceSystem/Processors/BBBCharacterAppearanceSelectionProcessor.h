#pragma once
struct FBBBCharacterAppearanceUpdateContext;
/** 角色外观的Selection职责 */
class FBBBCharacterAppearanceSelectionProcessor final
{
public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBCharacterAppearanceUpdateContext &Context) const;
};
