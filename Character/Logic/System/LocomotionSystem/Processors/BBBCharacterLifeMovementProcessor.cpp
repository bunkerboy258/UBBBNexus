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
        if (Phase == EBBBCharacterLifePhase::Alive)
        {
            const float OldHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
            const bool bCrouched = Context.Life.bRecoveryCrouched;
            const float HalfHeight = bCrouched ? Config.CrouchedHalfHeight : Config.CapsuleHalfHeight;
            Capsule->SetCapsuleSize(Config.CapsuleRadius, HalfHeight, true);
            Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            if (!Context.Execution.bIsMirror)
            {
                Character.AddActorWorldOffset(FVector(0, 0, (HalfHeight - OldHalfHeight) * Capsule->GetShapeScale()), false);
            }
            Character.bIsCrouched = bCrouched;
            Movement.bWantsToCrouch = bCrouched;
            const ACharacter *DefaultCharacter = Character.GetClass()->GetDefaultObject<ACharacter>();
            FVector Offset = DefaultCharacter->GetMesh()->GetRelativeLocation();
            Offset.Z += Config.CapsuleHalfHeight - HalfHeight;
            Character.GetMesh()->SetRelativeLocation(Offset);
            Character.CacheInitialMeshOffset(Offset, Character.GetMesh()->GetRelativeRotation());
            Movement.MinAnalogWalkSpeed = Config.MinAnalogWalkSpeed;
            Movement.MaxStepHeight = Config.MaxStepHeight;
            FFindFloorResult Floor;
            Movement.FindFloor(Character.GetActorLocation(), Floor, false);
            Movement.SetMovementMode(Floor.IsWalkableFloor() ? MOVE_Walking : MOVE_Falling);
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

    Movement.MaxWalkSpeed = 0.0f;
    Movement.MaxWalkSpeedCrouched = 0.0f;
    Movement.MinAnalogWalkSpeed = 0.0f;
    Movement.MaxAcceleration = 0.0f;
    Movement.Velocity.X = 0.0f;
    Movement.Velocity.Y = 0.0f;
    Character.StopJumping();
    Character.ConsumeMovementInputVector();
}
