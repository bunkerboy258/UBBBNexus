#pragma once
struct FBBBCharacterAppearanceUpdateContext;

/** 只为当前外观请求资源 等待期间保留已显示结果 */
class FBBBCharacterAppearanceLoadProcessor final
{
public:
    /** @param Context 当前外观依赖 @return 无 */
    void Update(FBBBCharacterAppearanceUpdateContext &Context) const;
    /** @param Context 当前外观依赖 @return 无 取消所属请求 */
    void Shutdown(FBBBCharacterAppearanceUpdateContext &Context) const;
};
