#pragma once

struct FBBBCharacterLocomotionUpdateContext;

/** 将救援操作许可应用到移动与朝向 */
class FBBBCharacterRescueMovementProcessor final
{
public:
    /** @param Context	本次移动上下文 @return 无 */
    void Update(FBBBCharacterLocomotionUpdateContext &Context) const;
};
