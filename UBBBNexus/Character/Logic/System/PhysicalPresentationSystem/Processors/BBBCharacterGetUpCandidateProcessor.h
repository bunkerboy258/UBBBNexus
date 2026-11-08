#pragma once
struct FBBBCharacterPhysicalPresentationUpdateContext;
/** 只判断物理身体是否具备安全起身条件 */
class FBBBCharacterGetUpCandidateProcessor final
{
public:
    /** @param Context 本帧依赖 @return 无 */
    void Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const;
};
