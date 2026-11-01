#pragma once
struct FBBBCharacterPhysicalPresentationUpdateContext;

/** 在物理完成后读取真实刚体结果 */
class FBBBCharacterRagdollCaptureProcessor final
{
public:
    /** @param Context 物理领域上下文 @return 无 */
    void Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const;
};
