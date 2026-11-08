#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterRagdollCaptureProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterRagdollCaptureProcessor::Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const
{
    auto &State = Context.Data.PhysicalPresentation.PhysicalPresentationState;
    auto &Snapshot = State.RagdollSnapshot;
    if (!Context.Data.External.ReadNetworkIdentityState().bIsMirror
        && Snapshot.Phase == EBBBCharacterPhysicalPhase::Recovering)
    {
        if (UAnimInstance *Anim = Context.Mesh.GetAnimInstance())
        {
            // 淡出中的实例仍提供播放位置 实例销毁后保留最后一个有效位置
            if (const FAnimMontageInstance *MontageInstance = Anim->GetInstanceForMontage(Snapshot.GetUpMontage))
            {
                Snapshot.GetUpPosition = MontageInstance->GetPosition();
            }
            Snapshot.RecoveryTransform = Context.Character.GetActorTransform();
            ++Snapshot.Revision;
        }
    }
    const FBodyInstance *Pelvis = State.bRagdoll ? Context.Mesh.GetBodyInstance(TEXT("pelvis")) : nullptr;
    if (!Pelvis)
    {
        return;
    }
    State.PelvisLocation = Pelvis->GetUnrealWorldTransform().GetLocation();
    if (!Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        Snapshot.PelvisLocation = State.PelvisLocation;
        Snapshot.Velocity = Pelvis->GetUnrealWorldVelocity();
        ++Snapshot.Revision;
    }
}
