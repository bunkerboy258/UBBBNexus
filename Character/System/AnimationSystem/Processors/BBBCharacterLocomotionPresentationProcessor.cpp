
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterLocomotionPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "GameFramework/CharacterMovementComponent.h"

//根据移动与瞄准状态推导移动表现枚举
void FBBBCharacterLocomotionPresentationProcessor::Update(
    const UCharacterMovementComponent &Movement,
    const FBBBCharacterLocomotionConfig &Config,
    const FBBBAimRuntimeData &AimData,
    const FBBBIntentRuntimeData &IntentData,
    FBBBCharacterAnimationState &AnimationState) const
{
    //读取水平速度与瞄准状态
    const float Speed = Movement.Velocity.Size2D();
    const bool bMoving = Speed >= 10.0f;
    const FBBBAimRuntimeState &AimState = AimData.GetState();
    const bool bAiming = AimState.bIsAiming;
    //默认待机
    AnimationState.LocomotionState = EBBBLocomotionState::Idle;
    //瞄准移动使用横移状态
    if (bMoving && bAiming)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::StrafeWalk;
        if (Speed > Config.StrafeWalkSpeed)
        {
            AnimationState.LocomotionState = EBBBLocomotionState::StrafeRun;
        }
    }
    //普通移动按速度分走跑
    if (bMoving && !bAiming)
    {
        AnimationState.LocomotionState = EBBBLocomotionState::Walk;
        if (Speed > Config.WalkSpeed)
        {
            AnimationState.LocomotionState = EBBBLocomotionState::Run;
        }
    }
    //提交移动输入供动画混合
    AnimationState.MoveInput = IntentData.GetMoveInput();
    AnimationState.SmoothedMoveInput = IntentData.GetSmoothedMoveInput();
}
