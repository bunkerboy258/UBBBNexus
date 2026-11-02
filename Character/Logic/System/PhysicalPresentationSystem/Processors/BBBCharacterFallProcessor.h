#pragma once
struct FBBBCharacterPhysicalPresentationUpdateContext;

/** 高处下落接管与首次身体撞地结算 */
class FBBBCharacterFallProcessor final
{
public:
    /** @param Context 本帧角色物理事实 @return 无 */
    void Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const;
};
