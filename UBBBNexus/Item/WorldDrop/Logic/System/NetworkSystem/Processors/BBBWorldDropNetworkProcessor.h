#pragma once
struct FBBBWorldDropNetworkUpdateContext;
/** 只读业务事实并投送复制与交付 */
class FBBBWorldDropNetworkProcessor final
{
public:
    /** @param Context\t本次更新依赖 @return 无 */
    void Update(FBBBWorldDropNetworkUpdateContext &Context) const;
};
