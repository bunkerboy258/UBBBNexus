#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/States/BBBCharacterPhysicalPresentationState.h"

/** 请求活着的角色进入并保持全身物理 不改变生命阶段 */
struct FBBBCharacterEnterRagdollLocalControlPacket final
{
    /** @return 无载荷请求始终格式有效 */
    bool IsValid() const
    {
        return true;
    }
    /** @param Context 当前事实 @return 是否允许接管身体 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.bInitialized
            && (Context.Life.bActionsAllowed
                || Context.PhysicalPresentation.RagdollSnapshot.Phase == EBBBCharacterPhysicalPhase::Recovering)
            && Context.Life.Phase == EBBBCharacterLifePhase::Alive
            && !Context.PhysicalPresentation.bRagdollRequested && !Context.PhysicalPresentation.bRagdoll;
    }
    /** @param Context 请求状态 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.PhysicalPresentation.bRagdollRequested = true;
        Context.PhysicalPresentation.bGetUpRequested = false;
        Context.PhysicalPresentation.bGetUpComplete = false;
    }
};
