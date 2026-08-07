
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "GameFramework/CharacterMovementComponent.h"

//根据装备姿态、速度档位与移动状态推导动画枚举
void FBBBCharacterLocomotionStateProcessor::Update(
    const UCharacterMovementComponent &Movement,
    float DeltaSeconds,
    const FBBBCharacterLocomotionConfig &Config,
    const FBBBAimRuntimeData &AimData,
    const FBBBFacingRuntimeData &FacingData,
    const FBBBIntentRuntimeData &IntentData,
    const FBBBCharacterEquipmentState &EquipmentState,
    FBBBAnimationRuntimeData &AnimationData,
    FBBBCharacterAnimationState &AnimationState) const
{
    //读取移动、瞄准与离地状态
    const float Speed = Movement.Velocity.Size2D();
    const bool bMoving = Speed >= 10.0f;
    const FBBBAimRuntimeState &AimState = AimData.GetState();
    const bool bAiming = AimState.bIsAiming;
    const bool bIsFalling = Movement.IsFalling();
    const bool bJustLanded = AnimationData.LocomotionPresentation.bWasFalling
        && !bIsFalling;
    const bool bStartedFalling = !AnimationData.LocomotionPresentation.bWasFalling
        && bIsFalling;

    //主手是否存在装备？
    const bool bHasRifle = EquipmentState.GetActiveMainHandInstance() != nullptr;

    //按当前装备姿态选择奔跑阈值
    float RunSpeedThreshold = Config.WalkSpeed;
    if (bHasRifle && bAiming)
    {
        RunSpeedThreshold = Config.StrafeWalkSpeed;
    }

    const float GaitSwitchHysteresis = FMath::Max(Config.AnimationGaitSwitchHysteresis, 0.0f);
    const float RunEnterSpeed = RunSpeedThreshold + GaitSwitchHysteresis;
    const float RunExitSpeed = FMath::Max(RunSpeedThreshold - GaitSwitchHysteresis, 0.0f);
    const bool bWasUsingRunAnimation = AnimationData.LocomotionPresentation.bUsesRunAnimation;
    bool bUsesRunAnimation = bWasUsingRunAnimation;
    if (!bMoving)
    {
        bUsesRunAnimation = false;
    }
    if (bMoving && !bWasUsingRunAnimation && Speed > RunEnterSpeed)
    {
        bUsesRunAnimation = true;
    }
    if (bMoving && bWasUsingRunAnimation && Speed < RunExitSpeed)
    {
        bUsesRunAnimation = false;
    }
    AnimationData.LocomotionPresentation.bUsesRunAnimation = bUsesRunAnimation;

    //原地开火视为进入原地瞄准
    const bool bIdleAiming = bAiming
        || IntentData.WantsFire();

    //按瞄准偏航方向读取原地转身状态
    const bool bTurningInPlaceLeft = FacingData.IsBodyTurning()
        && AimState.AimYaw < 0.0f;
    const bool bTurningInPlaceRight = FacingData.IsBodyTurning()
        && AimState.AimYaw > 0.0f;

    //首次离地时锁定整次跳跃的装备姿态与速度档位
    if (bStartedFalling)
    {
        AnimationData.LocomotionPresentation.bJumpStartedWithRifle = bHasRifle;
        AnimationData.LocomotionPresentation.bJumpStartedFromRun = bUsesRunAnimation;
        AnimationData.LocomotionPresentation.LandStateRemainingTime = 0.0f;
    }

    const float SafeDeltaSeconds = FMath::Max(DeltaSeconds, 0.0f);
    if (bJustLanded)
    {
        AnimationData.LocomotionPresentation.LandStateRemainingTime = FMath::Max(
            Config.AnimationLandStateDuration,
            SafeDeltaSeconds);
    }

    const bool bJumpStartedWithRifle = AnimationData.LocomotionPresentation.bJumpStartedWithRifle;
    const bool bJumpStartedFromRun = AnimationData.LocomotionPresentation.bJumpStartedFromRun;
    const bool bLandStateActive = !bIsFalling
        && AnimationData.LocomotionPresentation.LandStateRemainingTime > 0.0f;
    if (bLandStateActive)
    {
        AnimationData.LocomotionPresentation.LandStateRemainingTime = FMath::Max(
            AnimationData.LocomotionPresentation.LandStateRemainingTime - SafeDeltaSeconds,
            0.0f);
    }

    //提交移动表现跨帧状态供内部消费
    AnimationData.LocomotionPresentation.bWasFalling = bIsFalling;
    AnimationData.LocomotionPresentation.bIsTurningInPlaceLeft = bTurningInPlaceLeft;
    AnimationData.LocomotionPresentation.bIsTurningInPlaceRight = bTurningInPlaceRight;

    //提交移动输入供动画混合
    AnimationState.MoveInput = IntentData.GetMoveInput();
    AnimationState.SmoothedMoveInput = IntentData.GetSmoothedMoveInput();

    //落地保持期间维持起跳时的步枪速度档位
    if (bLandStateActive
        && bJumpStartedWithRifle
        && bJumpStartedFromRun)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleRunLand;
        return;
    }

    if (bLandStateActive
        && bJumpStartedWithRifle
        && !bJumpStartedFromRun)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleWalkLand;
        return;
    }

    if (bLandStateActive
        && !bJumpStartedWithRifle)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::EmptyHandLand;
        return;
    }

    //离地期间统一使用跳跃状态
    if (bIsFalling
        && bJumpStartedWithRifle
        && bJumpStartedFromRun)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleRunJump;
        return;
    }

    if (bIsFalling
        && bJumpStartedWithRifle
        && !bJumpStartedFromRun)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleWalkJump;
        return;
    }

    if (bIsFalling
        && !bJumpStartedWithRifle)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::EmptyHandJump;
        return;
    }

    //步枪原地瞄准向左转身
    if (bHasRifle && !bMoving && bIdleAiming && bTurningInPlaceLeft)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleIdleAimTurnLeft;
        return;
    }

    //步枪原地瞄准向右转身
    if (bHasRifle && !bMoving && bIdleAiming && bTurningInPlaceRight)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleIdleAimTurnRight;
        return;
    }

    //步枪原地瞄准
    if (bHasRifle && !bMoving && bIdleAiming)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleIdleAim;
        return;
    }

    //步枪瞄准横移超阈值升级奔跑
    if (bHasRifle && bMoving && bAiming && bUsesRunAnimation)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleStrafeRun;
        return;
    }

    //步枪瞄准移动使用横移状态
    if (bHasRifle && bMoving && bAiming)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleStrafeWalk;
        return;
    }

    //步枪非瞄准移动超阈值升级奔跑
    if (bHasRifle && bMoving && !bAiming && bUsesRunAnimation)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleRun;
        return;
    }

    //步枪非瞄准移动默认行走
    if (bHasRifle && bMoving && !bAiming)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleWalk;
        return;
    }

    //步枪默认待机
    if (bHasRifle)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::RifleIdle;
        return;
    }

    //空手移动超阈值使用奔跑
    if (bMoving && bUsesRunAnimation)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::EmptyHandRun;
        return;
    }

    //空手移动默认行走
    if (bMoving)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::EmptyHandWalk;
        return;
    }

    //空手默认待机
    AnimationState.LocomotionState = EBBBLocomotionState::EmptyHandIdle;
}
