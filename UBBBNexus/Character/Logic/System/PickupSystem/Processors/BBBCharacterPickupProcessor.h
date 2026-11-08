#pragma once
struct FBBBCharacterPickupUpdateContext;
/** 维护角色世界拾取的探测 请求与完成结果 */
class FBBBCharacterPickupProcessor final
{
public:
    /** @param Context\t本次更新依赖 @return 无 */
    void Update(FBBBCharacterPickupUpdateContext &Context) const;
    /** @param Context\t本次关闭依赖 @return 无 */
    void Shutdown(FBBBCharacterPickupUpdateContext &Context) const;
};
