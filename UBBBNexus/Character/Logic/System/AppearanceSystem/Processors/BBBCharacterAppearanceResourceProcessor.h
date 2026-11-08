#pragma once
struct FBBBCharacterAppearanceUpdateContext;

/** 准备请求和基础回退的模型资源 保留未变化材质缓存 */
class FBBBCharacterAppearanceResourceProcessor final
{
public:
    /** @param Context 本次外观依赖 @return 无 */
    void Update(FBBBCharacterAppearanceUpdateContext &Context) const;
};
