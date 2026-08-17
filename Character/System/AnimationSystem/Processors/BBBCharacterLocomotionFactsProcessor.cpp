#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionFactsProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
const FBBBCharacterLocomotionProfileConfig &ResolveLocomotionProfile(
    const FBBBAimRuntimeData &AimData,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterLocomotionConfig &LocomotionConfig)
{
    if (AimData.GetState().bIsAiming)
    {
        return LocomotionConfig.Strafe;
    }

    if (EquipmentState.GetActiveMainHandInstance())
    {
        return LocomotionConfig.MainHandEquipped;
    }

    return LocomotionConfig.Unarmed;
}

EBBBCharacterLocomotionMode ResolveLocomotionMode(
    const UCharacterMovementComponent &Movement,
    float GroundSpeed,
    const FBBBCharacterLocomotionProfileConfig &Profile)
{
    if (Movement.IsCrouching())
    {
        return EBBBCharacterLocomotionMode::Crouch;
    }

    if (GroundSpeed <= KINDA_SMALL_NUMBER)
    {
        return EBBBCharacterLocomotionMode::Idle;
    }

    //蹲伏除外，表示当前速度最接近走路档还是跑步档
    const float RunSpeedThreshold = (Profile.WalkSpeed + Profile.RunSpeed) * 0.5f;

    if (GroundSpeed < RunSpeedThreshold)
    {
        return EBBBCharacterLocomotionMode::Walk;
    }

    return EBBBCharacterLocomotionMode::Run;
}
}

void FBBBCharacterLocomotionFactsProcessor::Update(
    const UCharacterMovementComponent &Movement,
    const FBBBAimRuntimeData &AimData,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterLocomotionConfig &LocomotionConfig,
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
    float RawTurnRate = 0.0f;

    if (TurnTrackingState.bHasPreviousActorYaw
        && DeltaSeconds > KINDA_SMALL_NUMBER)
    {
        const float DeltaYaw = FMath::FindDeltaAngleDegrees(
            TurnTrackingState.PreviousActorYaw,
            CurrentActorYaw);

        RawTurnRate = DeltaYaw / DeltaSeconds;
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
    const FBBBCharacterLocomotionProfileConfig &LocomotionProfile = ResolveLocomotionProfile(
        AimData,
        EquipmentState,
        LocomotionConfig);
    const EBBBCharacterLocomotionMode LocomotionMode = ResolveLocomotionMode(
        Movement,
        GroundSpeed,
        LocomotionProfile);
    FBBBCharacterLocomotionTrackingState &LocomotionTracking = AnimationData.LocomotionTracking;

    if (!LocomotionTracking.bHasCurrentMode)
    {
        LocomotionTracking.CurrentMode = LocomotionMode;
        LocomotionTracking.PreviousMode = LocomotionMode;
        LocomotionTracking.bHasCurrentMode = true;
    }

    if (LocomotionTracking.CurrentMode != LocomotionMode)
    {
        LocomotionTracking.PreviousMode = LocomotionTracking.CurrentMode;
        LocomotionTracking.CurrentMode = LocomotionMode;
    }

    if (bIsMoving)
    {
        const float ForwardSpeedAbs = FMath::Abs(LocalVelocity.X);
        const float RightSpeedAbs = FMath::Abs(LocalVelocity.Y);

        if (ForwardSpeedAbs >= RightSpeedAbs
            && LocalVelocity.X >= 0.0f)
        {
            AnimationState.LastMoveDirection = EBBBCharacterMoveDirection::Forward;
        }

        if (ForwardSpeedAbs >= RightSpeedAbs
            && LocalVelocity.X < 0.0f)
        {
            AnimationState.LastMoveDirection = EBBBCharacterMoveDirection::Backward;
        }

        if (RightSpeedAbs > ForwardSpeedAbs
            && LocalVelocity.Y < 0.0f)
        {
            AnimationState.LastMoveDirection = EBBBCharacterMoveDirection::Left;
        }

        if (RightSpeedAbs > ForwardSpeedAbs
            && LocalVelocity.Y >= 0.0f)
        {
            AnimationState.LastMoveDirection = EBBBCharacterMoveDirection::Right;
        }
    }

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
    AnimationState.PreviousLocomotionMode = LocomotionTracking.PreviousMode;
    AnimationState.bHasMainHandEquipment = EquipmentState.GetActiveMainHandInstance() != nullptr;
    AnimationState.bIsTurningLeft = bIsTurningLeft;
    AnimationState.bIsTurningRight = bIsTurningRight;
    AnimationState.GroundSpeed = GroundSpeed;
    AnimationState.LocalForwardSpeed = LocalVelocity.X;
    AnimationState.LocalRightSpeed = LocalVelocity.Y;
    AnimationState.TurnRate = TurnRate;
    AnimationState.VerticalSpeed = Velocity.Z;
}
