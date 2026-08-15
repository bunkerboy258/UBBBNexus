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
    float ActorYawDelta = 0.0f;
    float RawTurnRate = 0.0f;

    if (TurnTrackingState.bHasPreviousActorYaw
        && DeltaSeconds > KINDA_SMALL_NUMBER)
    {
        ActorYawDelta = FMath::FindDeltaAngleDegrees(
            TurnTrackingState.PreviousActorYaw,
            CurrentActorYaw);

        RawTurnRate = ActorYawDelta / DeltaSeconds;
    }

    TurnTrackingState.PreviousActorYaw = CurrentActorYaw;
    TurnTrackingState.bHasPreviousActorYaw = true;

    //指数低通在不同帧率下保持相近响应并避免弹簧回弹
    const float TurnRateSmoothingAlpha = 1.0f - FMath::Exp(
        -FMath::Max(DeltaSeconds, 0.0f) / AnimationConfig.TurnRateSmoothingTime);
    const float FilteredTurnRate = FMath::Lerp(
        TurnTrackingState.SmoothedTurnRate,
        RawTurnRate,
        TurnRateSmoothingAlpha);

    //限制正负方向的转速变化幅度，避免动画输入因瞬时旋转发生突变
    TurnTrackingState.SmoothedTurnRate = FMath::FInterpConstantTo(
        TurnTrackingState.SmoothedTurnRate,
        FilteredTurnRate,
        DeltaSeconds,
        AnimationConfig.MaxTurnRateChangeSpeed);
    const float TurnRate = TurnTrackingState.SmoothedTurnRate;

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
    AnimationState.ActorYawDelta = ActorYawDelta;
    AnimationState.VerticalSpeed = Velocity.Z;
}
