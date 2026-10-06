#pragma once

struct FBBBCharacterLocomotionUpdateContext;

/** 注册和清理官方根运动校正目标 */
class FBBBCharacterTraversalWarpProcessor final
{
public:
    /** @param Context 本帧移动上下文 @return 无 */
    void Update(FBBBCharacterLocomotionUpdateContext &Context) const;
};
