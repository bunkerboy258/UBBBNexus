#pragma once
struct FBBBCharacterPhysicalPresentationUpdateContext;
/** 维护已批准的物理阶段并交还网格控制 */
class FBBBCharacterPhysicalPhaseProcessor final
{
public:
    /** @param Context 本帧依赖 @return 无 */
    void Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const;
};
