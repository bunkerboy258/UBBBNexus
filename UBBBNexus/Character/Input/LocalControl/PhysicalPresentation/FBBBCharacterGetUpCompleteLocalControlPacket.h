#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/States/BBBCharacterPhysicalPresentationState.h"

/** 起身蒙太奇自然到达通知终点后贡献轮次完成 */
struct FBBBCharacterGetUpCompleteLocalControlPacket final
{
    /** 同帧积累的通知轮次 */
    TArray<uint64> Episodes;
    /** @return 轮次是否全部有效 */
    bool IsValid() const
    {
        return !Episodes.IsEmpty() && !Episodes.Contains(0);
    }
    /** @param Context 当前事实 @return 是否仍处于可完成的起身 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.Phase == EBBBCharacterLifePhase::Alive
            && !Context.PhysicalPresentation.bRagdollRequested
            && Context.PhysicalPresentation.RagdollSnapshot.Phase == EBBBCharacterPhysicalPhase::Recovering
            && Episodes.Contains(Context.PhysicalPresentation.RagdollSnapshot.Episode);
    }
    /** @param Context 完成请求 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.PhysicalPresentation.bGetUpComplete = true;
    }
};
