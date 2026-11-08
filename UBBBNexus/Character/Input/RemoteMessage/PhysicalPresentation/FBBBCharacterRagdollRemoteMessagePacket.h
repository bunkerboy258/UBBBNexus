#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/States/BBBCharacterPhysicalPresentationState.h"

/** 还原当前布娃娃结果 不重新执行本机玩法 */
struct FBBBCharacterRagdollRemoteMessagePacket final
{
    /** 同帧积累的当前结果 */
    TArray<FBBBCharacterRagdollSnapshot> Snapshots;
    /** @return 数据是否完整有限 */
    bool IsValid() const
    {
        if (Snapshots.IsEmpty())
        {
            return false;
        }
        for (const auto &Snapshot : Snapshots)
        {
            if (Snapshot.Revision == 0 || Snapshot.Episode == 0 || Snapshot.Transition == 0
                || Snapshot.PelvisLocation.ContainsNaN() || Snapshot.Velocity.ContainsNaN()
                || Snapshot.RecoveryTransform.ContainsNaN() || !FMath::IsFinite(Snapshot.GetUpPosition)
                || Snapshot.GetUpPosition < 0.0f || uint8(Snapshot.Phase) > uint8(EBBBCharacterPhysicalPhase::Recovering)
                || (Snapshot.Phase == EBBBCharacterPhysicalPhase::Recovering && !Snapshot.GetUpMontage))
            {
                return false;
            }
        }
        return true;
    }
    /** @param Context 当前执行身份 @return 是否允许镜像还原 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.bIsMirror;
    }
    /** @param Context 物理领域状态 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        for (const auto &Snapshot : Snapshots)
        {
            if (Snapshot.Revision > Context.PhysicalPresentation.RagdollSnapshot.Revision)
            {
                Context.PhysicalPresentation.RagdollSnapshot = Snapshot;
                Context.PhysicalPresentation.bRagdollRequested = Snapshot.Phase == EBBBCharacterPhysicalPhase::Ragdoll
                    && !Context.PhysicalPresentation.bRagdoll;
            }
        }
    }
};
