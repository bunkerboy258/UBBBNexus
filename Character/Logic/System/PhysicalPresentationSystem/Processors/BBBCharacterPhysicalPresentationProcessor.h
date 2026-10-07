#pragma once
struct FBBBCharacterPhysicalPresentationUpdateContext;

/** 把既成生命与命中事实转换为骨骼物理表现 */
class FBBBCharacterPhysicalPresentationProcessor final
{
  public:
    /**
     * @param Context	本次只读事实与所属领域上下文
     * @return 无
     */
    void Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const;
};
