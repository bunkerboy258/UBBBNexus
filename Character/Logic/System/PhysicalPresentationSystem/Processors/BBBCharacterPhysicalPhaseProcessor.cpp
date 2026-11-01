#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterPhysicalPhaseProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Context/BBBCharacterPhysicalPresentationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterPhysicalPhaseProcessor::Update(FBBBCharacterPhysicalPresentationUpdateContext &Context) const
{
    auto &State = Context.Data.PhysicalPresentation.PhysicalPresentationState;
    auto &Result = State.RagdollSnapshot;
    const auto &Life = Context.Data.Life.ReadLifeState();
    const bool bMirror = Context.Data.External.ReadNetworkIdentityState().bIsMirror;
    if (!Life.bInitialized)
    {
        return;
    }
    if (!bMirror)
    {
        if (Life.Phase == EBBBCharacterLifePhase::Downed)
        {
            State.bWasDowned = true;
            if (Result.Phase == EBBBCharacterPhysicalPhase::Recovering)
            {
                State.bRagdollRequested = true;
            }
        }
        if (State.bWasDowned && Life.Phase == EBBBCharacterLifePhase::Alive && State.bRagdoll)
        {
            State.bGetUpRequested = true;
            State.bWasDowned = false;
        }
        if (State.bWasDowned && Life.Phase == EBBBCharacterLifePhase::Alive && !State.bRagdoll)
        {
            const auto &Config = Context.Character.GetCharacterConfig();
            UAnimMontage *Montage = Life.bRecoveryCrouched
                ? Config.Animation.RecoverCrouchMontage.Get() : Config.Animation.RecoverStandMontage.Get();
            if (ensureMsgf(Montage, TEXT("获救起身动作未配置")))
            {
                Context.Mesh.SnapshotPose(State.ExitPose);
                State.RagdollMeshTransform = Context.Mesh.GetComponentTransform();
                Result.RecoveryTransform = Context.Character.GetActorTransform();
                const auto *Capsule = Context.Character.GetCapsuleComponent();
                const float Height = Life.bRecoveryCrouched ? Config.Locomotion.CrouchedHalfHeight : Config.Locomotion.CapsuleHalfHeight;
                Result.RecoveryTransform.AddToTranslation(FVector(0, 0,
                    (Height - Capsule->GetUnscaledCapsuleHalfHeight()) * Capsule->GetShapeScale()));
                Result.bRecoveryCrouched = Life.bRecoveryCrouched;
                Result.Phase = EBBBCharacterPhysicalPhase::Recovering;
                Result.GetUpMontage = Montage;
                Result.GetUpPosition = 0.0f;
                ++Result.Episode;
                ++Result.Transition;
                ++Result.Revision;
                State.bWasDowned = false;
            }
        }
        if (State.bGetUpComplete && !State.bRagdollRequested && Life.Phase == EBBBCharacterLifePhase::Alive
            && Result.Phase == EBBBCharacterPhysicalPhase::Recovering)
        {
            Result.Phase = EBBBCharacterPhysicalPhase::Animated;
            Result.RecoveryTransform = Context.Character.GetActorTransform();
            ++Result.Transition;
            ++Result.Revision;
        }
        State.bGetUpComplete = false;
        const auto &Animation = Context.Data.Animation.ReadGetUpAnimationState();
        if (State.bRagdoll && State.bGetUpRequested && State.bCanGetUp && !State.bRagdollRequested
            && Life.Phase == EBBBCharacterLifePhase::Alive && Animation.SelectedMontage
            && Animation.SelectedEpisode == Result.Episode)
        {
            Result.Phase = EBBBCharacterPhysicalPhase::Recovering;
            Result.RecoveryTransform = State.GetUpTransform;
            Result.bRecoveryCrouched = false;
            Result.GetUpMontage = Animation.SelectedMontage;
            Result.GetUpPosition = Animation.SelectedTime;
            State.bGetUpRequested = false;
            ++Result.Transition;
            ++Result.Revision;
        }
    }
    if (State.bRagdoll && Result.Phase != EBBBCharacterPhysicalPhase::Ragdoll)
    {
        Context.Mesh.SnapshotPose(State.ExitPose);
        State.RagdollMeshTransform = Context.Mesh.GetComponentTransform();
        Context.Mesh.SetAllBodiesSimulatePhysics(false);
        Context.Mesh.SetSimulatePhysics(false);
        Context.Mesh.SetAllBodiesPhysicsBlendWeight(0.0f);
        Context.Mesh.AttachToComponent(Context.Character.GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
        const auto *DefaultMesh = Context.Character.GetClass()->GetDefaultObject<ABBBCharacter>()->GetMesh();
        Context.Mesh.SetCollisionProfileName(DefaultMesh->GetCollisionProfileName());
        Context.Mesh.bPauseAnims = false;
        State.bRagdoll = false;
        State.bRagdollRequested = false;
    }
    if (bMirror && !State.bRagdoll && Result.Phase == EBBBCharacterPhysicalPhase::Recovering
        && Context.Data.Locomotion.ReadLocomotionState().AppliedPhysicalTransition != Result.Transition)
    {
        Context.Mesh.SnapshotPose(State.ExitPose);
        State.RagdollMeshTransform = Context.Mesh.GetComponentTransform();
    }
    if (State.bRagdoll)
    {
        // 救援的业务完成必须让蒙太奇通知正常推进 物理仍完整覆盖骨骼输出
        Context.Mesh.bPauseAnims = !Context.Data.Life.ReadRescueState().bReceiving;
    }
}
