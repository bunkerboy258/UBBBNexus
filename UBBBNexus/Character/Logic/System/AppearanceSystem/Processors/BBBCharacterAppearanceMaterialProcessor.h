#pragma once
struct FBBBCharacterAppearanceUpdateContext;

/** 主体与附件的材质实例和参数的唯一维护者 */
class FBBBCharacterAppearanceMaterialProcessor final
{
public:
    /** @param Context 本次外观依赖 @return 无 */
    void Update(FBBBCharacterAppearanceUpdateContext &Context) const;
};
