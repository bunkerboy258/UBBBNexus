#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"

#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
// 将二维移动意图转换为控制器朝向下的世界方向
FVector ResolveWorldMoveDirection(
    const ACharacter &Character,
    const FBBBIntentRuntimeData &IntentData)
{
    const FRotator YawRotation(0.0f, Character.GetControlRotation().Yaw, 0.0f);
    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    return (Forward * IntentData.GetMoveInput().Y
        + Right * IntentData.GetMoveInput().X).GetSafeNormal2D();
}

// 检查冲刺意图和移动方向是否满足冲刺条件
bool CanSprint(
    const ACharacter &Character,
    const FBBBIntentRuntimeData &IntentData,
    const FBBBCharacterLocomotionConfig &Config)
{
    if (!IntentData.WantsSprint())
    {
        return false;
    }

    const FVector WorldMoveDirection = ResolveWorldMoveDirection(Character, IntentData);
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
    const FBBBIntentRuntimeData &IntentData,
    const FBBBCharacterLocomotionConfig &Config,
    EBBBCharacterGait PreviousGait,
    const FVector &CurrentVelocity)
{
    const bool bFullMovementInput = IntentData.GetMoveInput().Size() >= Config.AnalogRunThreshold;

    // ADS 使用步行档位，并优先于冲刺
    if (IntentData.WantsAim())
    {
        return EBBBCharacterGait::Walk;
    }

    // 松开输入后保持实际奔跑档位，直到地面制动结束
    if (!IntentData.HasMoveInput()
        && CurrentVelocity.SizeSquared2D() > FMath::Square(1.0f)
        && (PreviousGait == EBBBCharacterGait::Run || PreviousGait == EBBBCharacterGait::Sprint))
    {
        return PreviousGait;
    }

    if (CanSprint(Character, IntentData, Config) && bFullMovementInput)
    {
        return EBBBCharacterGait::Sprint;
    }

    if (IntentData.WantsWalk())
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
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    // 保存移动依赖并同步加载方向速度曲线
    Character = &InCharacter;
    Movement = &InMovement;
    RuntimeData = &InRuntimeData;
    IntentData = &InIntentData;
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
        Character && Movement && RuntimeData && IntentData && Config && StrafeSpeedMapCurve,
        TEXT("[UBBBC]Locomotion system update failed because dependencies are null")))
    {
        return;
    }

    const UWorld *World = Character->GetWorld();
    const float DeltaSeconds = World ? World->GetDeltaSeconds() : 0.0f;
    // 按世界帧间隔递减滑铲剩余时间
    SlideRemainingSeconds = FMath::Max(SlideRemainingSeconds - DeltaSeconds, 0.0f);

    // 地面滑铲意图建立新的滑铲方向和持续时间
    if (IntentData->WantsSlide() && Movement->IsMovingOnGround())
    {
        SlideRemainingSeconds = FMath::Max(Config->SlideDurationSeconds, 0.0f);
        SlideDirection = ResolveWorldMoveDirection(*Character, *IntentData);
        if (SlideDirection.IsNearlyZero())
        {
            SlideDirection = Character->GetActorForwardVector();
        }
    }

    const bool bSlideActive = SlideRemainingSeconds > 0.0f;
    const bool bWantsCrouch = IntentData->WantsCrouch() || bSlideActive;
    // 滑铲期间强制保持蹲伏状态
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
        *IntentData,
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

    if (bSlideActive)
    {
        Movement->MaxWalkSpeedCrouched = FMath::Max(Config->SlideSpeed, 1.0f);
    }
    Movement->MaxAcceleration = FMath::Max(Config->MaxAcceleration, 0.0f);
    Movement->BrakingDecelerationWalking = FMath::Max(Config->BrakingDeceleration, 0.0f);
    Movement->GroundFriction = FMath::Max(Config->GroundFriction, 0.0f);
    Movement->BrakingFriction = FMath::Max(Config->BrakingFriction, 0.0f);
    Movement->BrakingFrictionFactor = FMath::Max(Config->BrakingFrictionFactor, 0.0f);
    Movement->bUseSeparateBrakingFriction = false;

    // 地面冲刺使用移动方向没有方向时使用角色前方
    if (IntentData->WantsDash() && Movement->IsMovingOnGround())
    {
        FVector DashDirection = ResolveWorldMoveDirection(*Character, *IntentData);
        if (DashDirection.IsNearlyZero())
        {
            DashDirection = Character->GetActorForwardVector();
        }

        Character->LaunchCharacter(
            DashDirection * FMath::Max(Config->DashSpeed, 0.0f),
            false,
            false);
    }

    // 蹲伏期间不允许同帧跳跃
    if (!bWantsCrouch && IntentData->WantsJump())
    {
        Character->Jump();
    }

    // 滑铲使用固定方向并结束本帧移动处理
    if (bSlideActive)
    {
        Character->AddMovementInput(SlideDirection, 1.0f);
        return;
    }

    if (!IntentData->HasMoveInput())
    {
        return;
    }

    // 普通移动按控制器水平朝向写入两个方向分量
    const FRotator YawRotation(0.0f, Character->GetControlRotation().Yaw, 0.0f);
    Character->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X),
        IntentData->GetMoveInput().Y);
    Character->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y),
        IntentData->GetMoveInput().X);
}
