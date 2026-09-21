#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Processors/BBBCharacterLocomotionProcessor.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/States/BBBCharacterLocomotionState.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
// 将二维移动意图转换为控制器朝向下的世界方向
FVector ResolveWorldMoveDirection(
    const ACharacter &Character,
    const FBBBCharacterControlState &ControlData)
{
    return ControlData.MoveWorld.GetSafeNormal2D();
}

// 检查冲刺意图和移动方向是否满足冲刺条件
bool CanSprint(
    const ACharacter &Character,
    const FBBBCharacterControlState &ControlData,
    const FBBBCharacterLocomotionConfig &Config)
{
    if (!ControlData.bSprint)
    {
        return false;
    }

    const FVector WorldMoveDirection = ResolveWorldMoveDirection(Character, ControlData);
    if (WorldMoveDirection.IsNearlyZero())
    {
        return false;
    }

    const float DirectionYaw = WorldMoveDirection.Rotation().Yaw;
    const float DirectionDelta = FMath::Abs(FMath::FindDeltaAngleDegrees(
        Character.GetActorRotation().Yaw,
        DirectionYaw));
    return DirectionDelta < Config.SprintDirectionLimit;
}

// 根据瞄准和移动输入选择当前步态
EBBBCharacterGait ResolveGait(
    const ACharacter &Character,
    const FBBBCharacterControlState &ControlData,
    const FBBBCharacterLocomotionConfig &Config,
    EBBBCharacterGait PreviousGait,
    const FVector &CurrentVelocity)
{
    const bool bFullMovementInput = ControlData.MoveWorld.Size() >= Config.AnalogRunThreshold;

    // ADS 使用步行档位，并优先于冲刺
    if (ControlData.bAim)
    {
        return EBBBCharacterGait::Walk;
    }

    // 松开输入后保持实际奔跑档位，直到地面制动结束
    if (ControlData.MoveWorld.IsNearlyZero()
        && CurrentVelocity.SizeSquared2D() > FMath::Square(1.0f)
        && (PreviousGait == EBBBCharacterGait::Run || PreviousGait == EBBBCharacterGait::Sprint))
    {
        return PreviousGait;
    }

    if (CanSprint(Character, ControlData, Config) && bFullMovementInput)
    {
        return EBBBCharacterGait::Sprint;
    }

    if (ControlData.bWalk)
    {
        return EBBBCharacterGait::Walk;
    }

    if (bFullMovementInput)
    {
        return EBBBCharacterGait::Run;
    }

    return EBBBCharacterGait::Walk;
}

// 根据局部速度和方向曲线得到方向映射值
float ResolveDirectionMap(
    const ACharacter &Character,
    const UCharacterMovementComponent &Movement,
    const UCurveFloat &DirectionCurve)
{
    const FVector LocalVelocity = Character.GetActorTransform().InverseTransformVectorNoScale(
        Movement.Velocity);
    const float DirectionAngle = FMath::Abs(FMath::RadiansToDegrees(
        FMath::Atan2(LocalVelocity.Y, LocalVelocity.X)));
    return FMath::Clamp(DirectionCurve.GetFloatValue(DirectionAngle), 0.0f, 2.0f);
}

// 在不同方向速度之间进行插值
float ResolveDirectionalSpeed(const FVector &Speeds, const float DirectionMap)
{
    if (DirectionMap < 1.0f)
    {
        return FMath::Lerp(Speeds.X, Speeds.Y, DirectionMap);
    }

    return FMath::Lerp(Speeds.Y, Speeds.Z, DirectionMap - 1.0f);
}

// 根据步态选择速度配置并提供默认步行速度
float ResolveMaxSpeed(
    const FBBBCharacterLocomotionConfig &Config,
    const EBBBCharacterGait Gait,
    const float DirectionMap)
{
    switch (Gait)
    {
        case EBBBCharacterGait::Walk:
            return ResolveDirectionalSpeed(Config.WalkSpeeds, DirectionMap);

        case EBBBCharacterGait::Run:
            return ResolveDirectionalSpeed(Config.RunSpeeds, DirectionMap);

        case EBBBCharacterGait::Sprint:
            return ResolveDirectionalSpeed(Config.SprintSpeeds, DirectionMap);

        case EBBBCharacterGait::Crouch:
            return ResolveDirectionalSpeed(Config.CrouchSpeeds, DirectionMap);
    }

    return Config.WalkSpeeds.X;
}

}

void FBBBCharacterLocomotionProcessor::Update(
    FBBBCharacterLocomotionUpdateContext &Context) const
{
    ACharacter &Character = Context.Character;
    UCharacterMovementComponent &Movement = Context.Movement;
    FBBBCharacterLocomotionState &RuntimeData = Context.LocomotionState;
    const FBBBCharacterControlState &ControlData = Context.ControlState;
    const FBBBCharacterLocomotionConfig &Config = Context.Config;
    const UCurveFloat &StrafeSpeedMapCurve = Context.StrafeSpeedMapCurve;
    // 朝向来自外部提交的世界空间事实 不读取玩家控制器
    Character.SetActorRotation(FRotator(0.0f, ControlData.FacingWorld.Yaw, 0.0f));
    const bool bWantsCrouch = ControlData.bCrouch;
    if (bWantsCrouch)
    {
        Character.Crouch();
    }

    if (!bWantsCrouch)
    {
        Character.UnCrouch();
    }

    const EBBBCharacterGait PreviousGait = RuntimeData.Gait;
    EBBBCharacterGait Gait = ResolveGait(
        Character,
        ControlData,
        Config,
        PreviousGait,
        Movement.Velocity);
    if (bWantsCrouch)
    {
        Gait = EBBBCharacterGait::Crouch;
    }

    // 计算并提交当前步态和方向速度映射
    RuntimeData.Gait = Gait;
    const float DirectionMap = ResolveDirectionMap(
        Character,
        Movement,
        StrafeSpeedMapCurve);

    Movement.MaxWalkSpeed = FMath::Max(
        ResolveMaxSpeed(Config, Gait, DirectionMap),
        1.0f);
    Movement.MaxWalkSpeedCrouched = FMath::Max(
        ResolveDirectionalSpeed(Config.CrouchSpeeds, DirectionMap),
        1.0f);

    Movement.MaxAcceleration = FMath::Max(Config.MaxAcceleration, 0.0f);
    Movement.BrakingDecelerationWalking = FMath::Max(Config.BrakingDeceleration, 0.0f);
    Movement.GroundFriction = FMath::Max(Config.GroundFriction, 0.0f);
    Movement.BrakingFriction = FMath::Max(Config.BrakingFriction, 0.0f);
    Movement.BrakingFrictionFactor = FMath::Max(Config.BrakingFrictionFactor, 0.0f);
    Movement.bUseSeparateBrakingFriction = false;

    // 蹲伏期间不允许同帧跳跃
    if (!bWantsCrouch && ControlData.bJump)
    {
        Character.Jump();
    }


    if (ControlData.MoveWorld.IsNearlyZero())
    {
        return;
    }
    // 保留模拟输入强度 归一化只用于方向而不用于移动量
    const FVector MoveWorld = ControlData.MoveWorld;
    Character.AddMovementInput(MoveWorld.GetSafeNormal(), MoveWorld.Size());
}
