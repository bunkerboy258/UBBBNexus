#pragma once
struct FBBBWorldDropUpdateContext;
/** 维护落地 交付与本地表现 */
class FBBBWorldDropProcessor final
{
public:
    /** @param Context\t本次更新依赖 @return 无 */
    void Update(FBBBWorldDropUpdateContext &Context) const;
};
