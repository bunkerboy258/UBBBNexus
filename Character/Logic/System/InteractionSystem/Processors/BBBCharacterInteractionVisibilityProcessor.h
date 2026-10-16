#pragma once
struct FBBBCharacterInteractionUpdateContext;

/** 维护持有物品的网格隐藏状态 */
class FBBBCharacterInteractionVisibilityProcessor final
{
  public:
    /**
     * @param Context	本次只读事实与所属领域上下文
     * @return 无
     */
    void Update(FBBBCharacterInteractionUpdateContext &Context) const;
};
