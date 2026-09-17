#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"

#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
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
    return ControlData.GetMoveInput().GetSafeNormal2D();
}

// 检查冲刺意图和移动方向是否满足冲刺条件
bool CanSprint(
    const ACharacter &Character,
    const FBBBCharacterControlState &ControlData,
    const FBBBCharacterLocomotionConfig &Config)
{
    if (!ControlData.WantsSprint())
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
    const bool bFullMovementInput = ControlData.GetMoveInput().Size() >= Config.AnalogRunThreshold;

    // ADS 使用步行档位，并优先于冲刺
    if (ControlData.WantsAim())
    {
        return EBBBCharacterGait::Walk;
    }

    // 松开输入后保持实际奔跑档位，直到地面制动结束
    if (!ControlData.HasMoveInput()
        && CurrentVelocity.SizeSquared2D() > FMath::Square(1.0f)
        && (PreviousGait == EBBBCharacterGait::Run || PreviousGait == EBBBCharacterGait::Sprint))
    {
        return PreviousGait;
    }

    if (CanSprint(Character, ControlData, Config) && bFullMovementInput)
    {
        return EBBBCharacterGait::Sprint;
    }

    if (ControlData.WantsWalk())
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

void FBBBCharacterLocomotionSystem::Initialize(
    ACharacter &InCharacter,
    UCharacterMovementComponent &InMovement,
    FBBBCharacterLocomotionRuntimeData &InRuntimeData,
    const FBBBCharacterControlState &InIntentData,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    // 保存移动依赖并同步加载方向速度曲线
    Character = &InCharacter;
    Movement = &InMovement;
    RuntimeData = &InRuntimeData;
    ControlData = &InIntentData;
    Config = &InConfig;
    StrafeSpeedMapCurve = InConfig.StrafeSpeedMapCurve.LoadSynchronous();

    ensureMsgf(
        StrafeSpeedMapCurve,
        TEXT("[UBBBC]Locomotion initialization failed to load official strafe speed map curve"));
}

void FBBBCharacterLocomotionSystem::Update()
{
    // 移动更新需要角色组件意图状态和速度曲线全部有效
    if (!ensureMsgf(
        Character && Movement && RuntimeData && ControlData && Config && StrafeSpeedMapCurve,
        TEXT("[UBBBC]Locomotion system update failed because dependencies are null")))
    {
        return;
    }

    // 朝向来自外部提交的世界空间事实 不读取玩家控制器
    Character->SetActorRotation(FRotator(0.0f, ControlData->GetFacingWorld().Yaw, 0.0f));
    const bool bWantsCrouch = ControlData->WantsCrouch();
    if (bWantsCrouch)
    {
        Character->Crouch();
    }

    if (!bWantsCrouch)
    {
        Character->UnCrouch();
    }

    const EBBBCharacterGait PreviousGait = RuntimeData->GetGait();
    EBBBCharacterGait Gait = ResolveGait(
        *Character,
        *ControlData,
        *Config,
        PreviousGait,
        Movement->Velocity);
    if (bWantsCrouch)
    {
        Gait = EBBBCharacterGait::Crouch;
    }

    // 计算并提交当前步态和方向速度映射
    RuntimeData->CommitGait(Gait);
    const float DirectionMap = ResolveDirectionMap(
        *Character,
        *Movement,
        *StrafeSpeedMapCurve);

    Movement->MaxWalkSpeed = FMath::Max(
        ResolveMaxSpeed(*Config, Gait, DirectionMap),
        1.0f);
    Movement->MaxWalkSpeedCrouched = FMath::Max(
        ResolveDirectionalSpeed(Config->CrouchSpeeds, DirectionMap),
        1.0f);

    Movement->MaxAcceleration = FMath::Max(Config->MaxAcceleration, 0.0f);
    Movement->BrakingDecelerationWalking = FMath::Max(Config->BrakingDeceleration, 0.0f);
    Movement->GroundFriction = FMath::Max(Config->GroundFriction, 0.0f);
    Movement->BrakingFriction = FMath::Max(Config->BrakingFriction, 0.0f);
    Movement->BrakingFrictionFactor = FMath::Max(Config->BrakingFrictionFactor, 0.0f);
    Movement->bUseSeparateBrakingFriction = false;

    // 蹲伏期间不允许同帧跳跃
    if (!bWantsCrouch && ControlData->WantsJump())
    {
        Character->Jump();
    }


    if (!ControlData->HasMoveInput())
    {
        return;
    }
    // 保留模拟输入强度 归一化只用于方向而不用于移动量
    const FVector MoveWorld = ControlData->GetMoveInput();
    Character->AddMovementInput(MoveWorld.GetSafeNormal(), MoveWorld.Size());
}
