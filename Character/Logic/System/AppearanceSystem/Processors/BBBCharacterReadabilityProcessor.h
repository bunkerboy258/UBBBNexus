#pragma once
struct FBBBCharacterAppearanceUpdateContext;

/** 只维护本机角色的照明与可见轮廓标记 */
class FBBBCharacterReadabilityProcessor final
{
public:
    /** @param Context 所属外观依赖 @return 无 */
    void Update(FBBBCharacterAppearanceUpdateContext &Context) const;
    /** @param Context 所属外观依赖 @return 无 恢复部件并释放灯光 */
    void Shutdown(FBBBCharacterAppearanceUpdateContext &Context) const;
};
