#pragma once

struct FBBBCharacterLocomotionUpdateContext;

/** 生命阶段对碰撞体与移动的直接约束 */
class FBBBCharacterLifeMovementProcessor final
{
  public:
    /**
     * @param Context	本次只读事实与所属领域上下文
     * @return 无
     */
    void Update(FBBBCharacterLocomotionUpdateContext &Context) const;
};
