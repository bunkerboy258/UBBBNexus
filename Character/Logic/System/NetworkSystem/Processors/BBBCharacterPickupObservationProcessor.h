#pragma once
struct FBBBCharacterNetworkUpdateContext;
/** 只观察拾取请求与入包结果并按角色连接投送 */
class FBBBCharacterPickupObservationProcessor final
{
public:
    /** @param Context\t本次角色网络更新依赖 @return 无 */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
