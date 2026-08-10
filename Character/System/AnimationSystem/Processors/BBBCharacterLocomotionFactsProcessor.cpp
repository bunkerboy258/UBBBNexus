#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionFactsProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionFactsProcessor::Update(
    const UCharacterMovementComponent &Movement,
    const FBBBCharacterEquipmentState &EquipmentState,
    float DeltaSeconds,
    FBBBAnimationRuntimeData &AnimationData,
    FBBBCharacterAnimationState &AnimationState) const
{
    const AActor *Owner = Movement.GetOwner();
    if (!ensureMsgf(Owner, TEXT("[UBBBC]Locomotion facts update failed because movement owner is null")))
    {
        return;
    }

    const FVector Velocity = Movement.Velocity;
    const FVector LocalVelocity = Owner->GetActorTransform().InverseTransformVectorNoScale(Velocity);
    const float GroundSpeed = Velocity.Size2D();
    const float CurrentActorYaw = Owner->GetActorRotation().Yaw;
    FBBBCharacterTurnTrackingState &TurnTrackingState = AnimationData.TurnTracking;
    float TurnRate = 0.0f;

    if (TurnTrackingState.bHasPreviousActorYaw && DeltaSeconds > KINDA_SMALL_NUMBER)
    {
        const float DeltaYaw = FMath::FindDeltaAngleDegrees(
            TurnTrackingState.PreviousActorYaw,
            CurrentActorYaw);

        TurnRate = DeltaYaw / DeltaSeconds;
    }

    TurnTrackingState.PreviousActorYaw = CurrentActorYaw;
    TurnTrackingState.bHasPreviousActorYaw = true;

    AnimationState.bIsMoving = GroundSpeed > KINDA_SMALL_NUMBER;
    AnimationState.bIsGrounded = Movement.IsMovingOnGround();
    AnimationState.bIsCrouching = Movement.IsCrouching();
    AnimationState.bHasMainHandEquipment = EquipmentState.GetActiveMainHandInstance() != nullptr;
    AnimationState.GroundSpeed = GroundSpeed;
    AnimationState.LocalForwardSpeed = LocalVelocity.X;
    AnimationState.LocalRightSpeed = LocalVelocity.Y;
    AnimationState.TurnRate = TurnRate;
    AnimationState.VerticalSpeed = Velocity.Z;
}
