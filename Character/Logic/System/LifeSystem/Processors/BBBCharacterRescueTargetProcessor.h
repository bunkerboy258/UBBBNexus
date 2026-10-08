#pragma once

struct FBBBCharacterLifeUpdateContext;

/** 维护角色救援的接受与计时 */
class FBBBCharacterRescueTargetProcessor final
{
public:
    /** @param Context	本次生命上下文 @return 无 */
    void Update(FBBBCharacterLifeUpdateContext &Context) const;
};
