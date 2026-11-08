#pragma once
struct FBBBCharacterNetworkUpdateContext;
/** 只观察已经成立的外观结果 */
class FBBBCharacterAppearanceObservationProcessor final
{
public:
    /** @param Context 网络观察依赖 @return 无 */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};
