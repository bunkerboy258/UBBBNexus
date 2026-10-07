#pragma once
struct FBBBCharacterEquipmentUpdateContext;

/** 在持有关系之外维护临时收起及恢复边沿 */
class FBBBCharacterEquipmentUseProcessor final
{
  public:
    /**
     * @param Context	本次只读事实与所属领域上下文
     * @return 无
     */
    void Update(FBBBCharacterEquipmentUpdateContext &Context) const;
};
