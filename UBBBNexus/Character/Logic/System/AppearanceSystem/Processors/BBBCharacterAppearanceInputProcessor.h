#pragma once
struct FBBBCharacterAppearanceUpdateContext;
/** 角色外观的Input职责 */
class FBBBCharacterAppearanceInputProcessor final
{
public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBCharacterAppearanceUpdateContext &Context) const;
    /** @param Context	关闭依赖 @return 无 */
    static void Shutdown(FBBBCharacterAppearanceUpdateContext &Context);
};
