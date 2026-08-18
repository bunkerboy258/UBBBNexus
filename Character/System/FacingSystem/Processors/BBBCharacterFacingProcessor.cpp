#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Processors/BBBCharacterFacingProcessor.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Facing/BBBCharacterFacingConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/States/BBBCharacterFacingStates.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
constexpr float IdleSpeedThreshold = 1.0f;

void StopIdleTurn(FBBBCharacterFacingState &State)
{
    State.bIsIdleTurnActive = false;
}

void UseMovementDirection(UCharacterMovementComponent &Movement)
{
    Movement.bOrientRotationToMovement = true;
    Movement.bUseControllerDesiredRotation = false;
}

void UseCameraDirection(
    UCharacterMovementComponent &Movement,
    float RotationRate)
{
    Movement.bOrientRotationToMovement = false;
    Movement.bUseControllerDesiredRotation = true;
    Movement.RotationRate.Yaw = RotationRate;
}
}

void FBBBCharacterFacingProcessor::Update(
    ACharacter &Character,
    UCharacterMovementComponent &Movement,
    const FBBBIntentRuntimeData &IntentData,
    const FBBBCharacterFacingConfig &Config,
    FBBBCharacterFacingState &State) const
{
    //非瞄准移动保持引擎原生移动朝向
    if (IntentData.HasMoveInput()
        && !IntentData.WantsAim()
        && !IntentData.WantsFire())
    {
        StopIdleTurn(State);
        UseMovementDirection(Movement);
        return;
    }

    //非瞄准原地状态不执行身体追赶
    if (!IntentData.WantsAim()
        && !IntentData.WantsFire())
    {
        StopIdleTurn(State);
        UseMovementDirection(Movement);
        return;
    }

    //移动期间不经过原地滞回，持续快速追赶相机水平朝向
    if (IntentData.HasMoveInput())
    {
        StopIdleTurn(State);
        UseCameraDirection(Movement, Config.MoveAlignmentRotationRate);
        return;
    }

    const bool bIsIdle = Movement.IsMovingOnGround()
        && Movement.Velocity.SizeSquared2D() <= FMath::Square(IdleSpeedThreshold);

    //减速或离地期间保持引擎原生移动朝向，不触发原地转身
    if (!bIsIdle)
    {
        StopIdleTurn(State);
        UseMovementDirection(Movement);
        return;
    }

    //朝向系统只使用角度差绝对值控制滞回，不对外解释转身方向
    const float CameraYawDeltaAbs = FMath::Abs(FMath::FindDeltaAngleDegrees(
        Character.GetActorRotation().Yaw,
        Character.GetControlRotation().Yaw));
    //本地静止角色尚未启动追赶且相机仍在允许区间内时保持身体朝向
    if (!State.bIsIdleTurnActive
        && CameraYawDeltaAbs <= Config.IdleTurnStartAngle)
    {
        StopIdleTurn(State);
        UseMovementDirection(Movement);
        return;
    }

    //已经启动追赶并进入停止区间后结束旋转，避免阈值附近反复启停
    if (State.bIsIdleTurnActive
        && CameraYawDeltaAbs <= Config.IdleTurnStopAngle)
    {
        StopIdleTurn(State);
        UseMovementDirection(Movement);
        return;
    }

    //FacingSystem只决定是否追赶本地控制器，不生成任何动画方向事实
    State.bIsIdleTurnActive = true;

    UseCameraDirection(Movement, Config.IdleTurnRotationRate);
}
