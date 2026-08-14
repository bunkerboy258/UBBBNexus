#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Processors/BBBCharacterFacingProcessor.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Facing/BBBCharacterFacingConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/States/BBBCharacterFacingStates.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
constexpr float IdleSpeedThreshold = 1.0f;

void ClearTurnSignals(FBBBCharacterFacingState &State)
{
    State.bIsTurningLeft = false;
    State.bIsTurningRight = false;
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
    //移动期间不经过原地滞回，持续快速追赶相机水平朝向
    if (IntentData.HasMoveInput())
    {
        ClearTurnSignals(State);
        UseCameraDirection(Movement, Config.MoveAlignmentRotationRate);
        return;
    }

    const bool bIsIdle = Movement.IsMovingOnGround()
        && Movement.Velocity.SizeSquared2D() <= FMath::Square(IdleSpeedThreshold);

    //减速或离地期间保持引擎原生移动朝向，不触发原地转身
    if (!bIsIdle)
    {
        ClearTurnSignals(State);
        UseMovementDirection(Movement);
        return;
    }

    const float CameraYawDelta = FMath::FindDeltaAngleDegrees(
        Character.GetActorRotation().Yaw,
        Character.GetControlRotation().Yaw);
    const float CameraYawDeltaAbs = FMath::Abs(CameraYawDelta);
    const bool bHasActiveTurnSignal = State.bIsTurningLeft || State.bIsTurningRight;

    //尚未转身且相机仍在启动区间内时保持角色方向
    if (!bHasActiveTurnSignal
        && CameraYawDeltaAbs <= Config.IdleTurnStartAngle)
    {
        ClearTurnSignals(State);
        UseMovementDirection(Movement);
        return;
    }

    //已经转身并进入停止区间后结束旋转，避免阈值附近反复切换
    if (bHasActiveTurnSignal
        && CameraYawDeltaAbs <= Config.IdleTurnStopAngle)
    {
        ClearTurnSignals(State);
        UseMovementDirection(Movement);
        return;
    }

    State.bIsTurningLeft = false;
    State.bIsTurningRight = true;

    //负偏角表示相机位于角色左侧
    if (CameraYawDelta < 0.0f)
    {
        State.bIsTurningLeft = true;
        State.bIsTurningRight = false;
    }

    UseCameraDirection(Movement, Config.IdleTurnRotationRate);
}
