#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterLifeMovementProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"

void FBBBCharacterLifeMovementProcessor::Update(FBBBCharacterLocomotionUpdateContext &Context) const
{
    ACharacter &Character = Context.Character;
    UCharacterMovementComponent &Movement = Context.Movement;
    auto &State = Context.Data.Locomotion.LocomotionState;
    const auto Phase = Context.Life.Phase;
    const auto &Config = Context.Config;
    if (Phase != State.AppliedLifePhase)
    {
        Character.StopJumping();
        Character.ConsumeMovementInputVector();
        Character.UnCrouch();
        if (Character.bIsCrouched)
        {
            Movement.UnCrouch(Context.Execution.bIsMirror);
        }
        Movement.StopMovementImmediately();
        Movement.ClearAccumulatedForces();
        Movement.CurrentRootMotion.Clear();
        Movement.RootMotionParams.Clear();
        State.bRun = false;
        State.bTraversalControlled = false;
        State.TraversalEntrySpeed = 0.0f;
        UCapsuleComponent *Capsule = Character.GetCapsuleComponent();
        if (Phase == EBBBCharacterLifePhase::Downed)
        {
            const float OldHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
            const float Radius = FMath::Max(1.0f, Config.DownedCapsuleRadius);
            const float HalfHeight = FMath::Max(Radius, Config.DownedCapsuleHalfHeight);
            const float Delta = OldHalfHeight - HalfHeight;
            Capsule->SetCapsuleSize(Radius, HalfHeight, true);
            Character.bIsCrouched = false;
            Movement.bWantsToCrouch = false;
            if (!Context.Execution.bIsMirror)
            {
                Character.AddActorWorldOffset(FVector(0, 0, -Delta * Capsule->GetShapeScale()), false);
            }
            USkeletalMeshComponent *Mesh = Character.GetMesh();
            // 默认网格偏移不包含蹲伏调整 低顶阻止站起时仍能保持倒地底部
            const ACharacter *DefaultCharacter = Character.GetClass()->GetDefaultObject<ACharacter>();
            FVector Offset = DefaultCharacter->GetMesh()->GetRelativeLocation();
            Offset.Z += Config.CapsuleHalfHeight - HalfHeight;
            Mesh->SetRelativeLocation(Offset);
            Character.CacheInitialMeshOffset(Offset, Mesh->GetRelativeRotation());
            Movement.SetMovementMode(MOVE_Walking);
        }
        if (Phase == EBBBCharacterLifePhase::Dead)
        {
            Movement.DisableMovement();
            Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        State.AppliedLifePhase = Phase;
    }
    if (Phase == EBBBCharacterLifePhase::Dead)
    {
        Movement.DisableMovement();
    }
    if (Phase != EBBBCharacterLifePhase::Downed)
    {
        return;
    }

    Movement.MaxWalkSpeed = FMath::Max(0.0f, Config.DownedSpeed);
    Movement.MaxWalkSpeedCrouched = Movement.MaxWalkSpeed;
    Movement.MinAnalogWalkSpeed = 0.0f;
    Movement.MaxStepHeight = FMath::Min(Config.MaxStepHeight, 15.0f);
    Movement.Velocity = Movement.Velocity.GetClampedToMaxSize2D(Movement.MaxWalkSpeed);
    if (Context.Execution.bIsMirror)
    {
        return;
    }

    const float YawDelta =
        FMath::FindDeltaAngleDegrees(Character.GetActorRotation().Yaw, Context.ControlState.FacingWorld.Yaw);
    const float Limit = Config.DownedTurnSpeed * Context.DeltaSeconds;
    Character.SetActorRotation(
        FRotator(0, Character.GetActorRotation().Yaw + FMath::Clamp(YawDelta, -Limit, Limit), 0));
    const FVector Intent = Context.ControlState.MoveWorld.GetClampedToMaxSize2D(1.0f);
    Character.AddMovementInput(Intent.GetSafeNormal2D(), Intent.Size2D());
}
