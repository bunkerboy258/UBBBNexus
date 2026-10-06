#pragma once

struct FBBBCharacterLocomotionUpdateContext;

/** 维护翻越启动超时与结束恢复条件 */
class FBBBCharacterTraversalLifeProcessor final
{
public:
    /** @param Context 本帧移动上下文 @return 无 */
    void Update(FBBBCharacterLocomotionUpdateContext &Context) const;
};
