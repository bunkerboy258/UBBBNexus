#pragma once
struct FBBBWorldDropUpdateContext;
/** 只读落地结果并维护本地光柱与图标 */
class FBBBWorldDropPresentationProcessor final
{
public:
    /** @param Context\t本次更新依赖 @return 无 */
    void Update(FBBBWorldDropUpdateContext &Context) const;
};
