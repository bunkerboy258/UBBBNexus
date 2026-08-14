#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionFactsProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionFactsProcessor::Update(
    const UCharacterMovementComponent &Movement,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterAnimationConfig &AnimationConfig,
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

    if (TurnTrackingState.bHasPreviousActorYaw
        && TurnTrackingState.PreviousMovementDeltaSeconds > KINDA_SMALL_NUMBER)
    {
        const float DeltaYaw = FMath::FindDeltaAngleDegrees(
            TurnTrackingState.PreviousActorYaw,
            CurrentActorYaw);

        TurnRate = DeltaYaw / TurnTrackingState.PreviousMovementDeltaSeconds;
    }

    TurnTrackingState.PreviousActorYaw = CurrentActorYaw;
    TurnTrackingState.PreviousMovementDeltaSeconds = DeltaSeconds;
    TurnTrackingState.bHasPreviousActorYaw = true;

    const bool bIsMoving = GroundSpeed > KINDA_SMALL_NUMBER;
    const bool bIsGrounded = Movement.IsMovingOnGround();

    //动画系统只根据角色本体实际转速生成方向信号，不读取相机或控制器
    const bool bIsTurningLeft = bIsGrounded
        && !bIsMoving
        && TurnRate < -AnimationConfig.TurnSignalRateThreshold;
    const bool bIsTurningRight = bIsGrounded
        && !bIsMoving
        && TurnRate > AnimationConfig.TurnSignalRateThreshold;

    AnimationState.bIsMoving = bIsMoving;
    AnimationState.bIsGrounded = bIsGrounded;
    AnimationState.bIsCrouching = Movement.IsCrouching();
    AnimationState.bHasMainHandEquipment = EquipmentState.GetActiveMainHandInstance() != nullptr;
    AnimationState.bIsTurningLeft = bIsTurningLeft;
    AnimationState.bIsTurningRight = bIsTurningRight;
    AnimationState.GroundSpeed = GroundSpeed;
    AnimationState.LocalForwardSpeed = LocalVelocity.X;
    AnimationState.LocalRightSpeed = LocalVelocity.Y;
    AnimationState.TurnRate = TurnRate;
    AnimationState.VerticalSpeed = Velocity.Z;
}
