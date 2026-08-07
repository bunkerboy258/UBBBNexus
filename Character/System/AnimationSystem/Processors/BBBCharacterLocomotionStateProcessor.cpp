
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "GameFramework/CharacterMovementComponent.h"

//根据移动与瞄准状态推导移动表现枚举
void FBBBCharacterLocomotionStateProcessor::Update(
    const UCharacterMovementComponent &Movement,
    const FBBBCharacterLocomotionConfig &Config,
    const FBBBAimRuntimeData &AimData,
    const FBBBFacingRuntimeData &FacingData,
    const FBBBIntentRuntimeData &IntentData,
    FBBBAnimationRuntimeData &AnimationData,
    FBBBCharacterAnimationState &AnimationState) const
{
    //读取水平速度与瞄准状态
    const float Speed = Movement.Velocity.Size2D();
    const bool bMoving = Speed >= 10.0f;
    const FBBBAimRuntimeState &AimState = AimData.GetState();
    const bool bAiming = AimState.bIsAiming;

    //原地开火视为进入原地瞄准
    const bool bIdleAiming = bAiming
        || IntentData.WantsFire();

    //按瞄准偏航方向读取原地转身状态
    const bool bTurningInPlaceLeft = FacingData.IsBodyTurning()
        && AimState.AimYaw < 0.0f;
    const bool bTurningInPlaceRight = FacingData.IsBodyTurning()
        && AimState.AimYaw > 0.0f;

    //默认待机
    AnimationState.LocomotionState = EBBBLocomotionState::Idle;

    //原地瞄准
    if (!bMoving && bIdleAiming)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::IdleAim;
    }

    //原地瞄准向左转身
    if (!bMoving && bIdleAiming && bTurningInPlaceLeft)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::IdleAimTurnLeft;
    }

    //原地瞄准向右转身
    if (!bMoving && bIdleAiming && bTurningInPlaceRight)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::IdleAimTurnRight;
    }

    //瞄准移动使用横移状态
    if (bMoving && bAiming)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::StrafeWalk;
    }

    //横移超阈值升级奔跑
    if (bMoving && bAiming && Speed > Config.StrafeWalkSpeed)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::StrafeRun;
    }

    //普通移动默认行走
    if (bMoving && !bAiming)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::Walk;
    }

    //移动超阈值升级奔跑
    if (bMoving && !bAiming && Speed > Config.WalkSpeed)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::Run;
    }

    //提交原地转身运行时状态供内部消费
    AnimationData.LocomotionPresentation.bIsTurningInPlaceLeft = bTurningInPlaceLeft;
    AnimationData.LocomotionPresentation.bIsTurningInPlaceRight = bTurningInPlaceRight;

    //提交移动输入供动画混合
    AnimationState.MoveInput = IntentData.GetMoveInput();
    AnimationState.SmoothedMoveInput = IntentData.GetSmoothedMoveInput();
}

