#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/States/BBBCharacterPhysicalPresentationState.h"

/** 请求活着的布娃娃在满足空间条件后起身 不替代救援 */
struct FBBBCharacterRequestGetUpLocalControlPacket final
{
    /** @return 无载荷请求有效 */
    bool IsValid() const { return true; }
    /** @param Context 当前事实 @return 是否允许提出起身请求 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.bInitialized
            && Context.Life.Phase == EBBBCharacterLifePhase::Alive
            && Context.PhysicalPresentation.RagdollSnapshot.Phase == EBBBCharacterPhysicalPhase::Ragdoll
            && !Context.PhysicalPresentation.bRagdollRequested;
    }
    /** @param Context 请求状态 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.PhysicalPresentation.bGetUpRequested = true;
    }
};
