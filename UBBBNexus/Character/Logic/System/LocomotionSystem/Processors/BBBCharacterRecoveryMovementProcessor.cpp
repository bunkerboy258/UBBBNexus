#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterRecoveryMovementProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterRecoveryMovementProcessor::Update(FBBBCharacterLocomotionUpdateContext &Context) const
{
    const auto &Physical = Context.Data.PhysicalPresentation.ReadPhysicalPresentationState();
    const auto &Result = Physical.RagdollSnapshot;
    auto &State = Context.Data.Locomotion.LocomotionState;
    auto &Character = Context.Character;
    auto &Movement = Context.Movement;
    if (State.AppliedPhysicalTransition != Result.Transition)
    {
        State.AppliedPhysicalTransition = Result.Transition;
        if (Result.Phase != EBBBCharacterPhysicalPhase::Ragdoll)
        {
            auto *Capsule = Character.GetCapsuleComponent();
            auto *Mesh = Character.GetMesh();
            const auto *Defaults = Character.GetClass()->GetDefaultObject<ACharacter>();
            const float Height = Result.bRecoveryCrouched ? Context.Config.CrouchedHalfHeight : Context.Config.CapsuleHalfHeight;
            Capsule->SetCapsuleSize(Context.Config.CapsuleRadius, Height, false);
            Character.SetActorTransform(Result.RecoveryTransform, false, nullptr, ETeleportType::TeleportPhysics);
            Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Character.bIsCrouched = Result.bRecoveryCrouched;
            Movement.bWantsToCrouch = Result.bRecoveryCrouched;
            FTransform Offset = Defaults->GetMesh()->GetRelativeTransform();
            Offset.AddToTranslation(FVector(0, 0, Context.Config.CapsuleHalfHeight - Height));
            Mesh->SetRelativeTransform(Offset, false, nullptr, ETeleportType::TeleportPhysics);
            Character.CacheInitialMeshOffset(Offset.GetLocation(), Offset.Rotator());
            Movement.StopMovementImmediately();
            Movement.ClearAccumulatedForces();
            Movement.CurrentRootMotion.Clear();
            Movement.RootMotionParams.Clear();
            Movement.NetworkSmoothingMode = Defaults->GetCharacterMovement()->NetworkSmoothingMode;
            Movement.MinAnalogWalkSpeed = Context.Config.MinAnalogWalkSpeed;
            Movement.MaxStepHeight = Context.Config.MaxStepHeight;
            Movement.SetMovementMode(MOVE_Walking);
            State.AppliedLifePhase = Context.Life.Phase;
        }
    }
    if (Result.Phase == EBBBCharacterPhysicalPhase::Recovering)
    {
        Character.StopJumping();
        Character.ConsumeMovementInputVector();
        Movement.bUseControllerDesiredRotation = false;
        Movement.MaxAcceleration = 0.0f;
        State.bRun = false;
    }
}
