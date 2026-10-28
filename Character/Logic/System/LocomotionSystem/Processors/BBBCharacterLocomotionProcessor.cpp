#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterLocomotionProcessor.h"

#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "Curves/CurveFloat.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeState.h"

namespace
{
// 将二维移动意图转换为控制器朝向下的世界方向
FVector ResolveWorldMoveDirection(
    const ACharacter &Character,
    const FBBBCharacterControlState &ControlData)
{
    return ControlData.MoveWorld.GetSafeNormal2D();
}

// 检查跑步意图和移动方向是否满足跑步条件
bool CanRun(
    const ACharacter &Character,
    const FBBBCharacterControlState &ControlData,
    const FBBBCharacterLocomotionConfig &Config)
{
    if (!ControlData.bRun)
    {
        return false;
    }

    const FVector WorldMoveDirection = ResolveWorldMoveDirection(Character, ControlData);
    if (WorldMoveDirection.IsNearlyZero())
    {
        return false;
    }

    const float DirectionYaw = WorldMoveDirection.Rotation().Yaw;
    // 跑步资格按输入目标朝向裁决 避免胶囊转向滞后造成档位切换
    const float DirectionDelta = FMath::Abs(FMath::FindDeltaAngleDegrees(
        ControlData.FacingWorld.Yaw,
        DirectionYaw));
    return DirectionDelta < Config.RunDirectionLimit;
}

// 松开移动后保留制动阶段的跑步状态 避免动画提前退回步行
bool ResolveRun(
    const ACharacter &Character,
    const FBBBCharacterControlState &ControlData,
    const FBBBCharacterLocomotionConfig &Config,
    const bool bWasRunning,
    const FVector &CurrentVelocity)
{
    const bool bFullMovementInput = ControlData.MoveWorld.Size() >= Config.AnalogRunThreshold;

    if (ControlData.bAim || ControlData.bCrouch)
    {
        return false;
    }

    if (ControlData.MoveWorld.IsNearlyZero()
        && CurrentVelocity.SizeSquared2D() > FMath::Square(1.0f)
        && bWasRunning)
    {
        return true;
    }

    return bFullMovementInput && CanRun(Character, ControlData, Config);
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

// 根据当前跑步状态选择方向速度
float ResolveMaxSpeed(
    const FBBBCharacterLocomotionConfig &Config,
    const bool bRun,
    const float DirectionMap)
{
    if (bRun)
    {
        return ResolveDirectionalSpeed(Config.RunSpeeds, DirectionMap);
    }

    return ResolveDirectionalSpeed(Config.WalkSpeeds, DirectionMap);
}

}

void FBBBCharacterLocomotionProcessor::Update(
    FBBBCharacterLocomotionUpdateContext &Context) const
{
    ACharacter &Character = Context.Character;
    UCharacterMovementComponent &Movement = Context.Movement;
    FBBBCharacterLocomotionState &RuntimeData = Context.LocomotionState;
    if (!Context.Life.bActionsAllowed)
    {
        return;
    }
    const FBBBCharacterControlState &ControlData = Context.ControlState;
    const FBBBCharacterLocomotionConfig &Config = Context.Config;
    const UCurveFloat &StrafeSpeedMapCurve = Context.StrafeSpeedMapCurve;
    // 镜像使用 CMC 已接收的控制朝向还原旋转 地面保持即时到达 空中保留追向速率
    Movement.bUseControllerDesiredRotation = Context.Execution.bIsMirror;
    Movement.RotationRate.Yaw = 360.0f;
    if (Context.Execution.bIsMirror && Movement.IsMovingOnGround())
    {
        Movement.RotationRate.Yaw = -1.0f;
    }

    // 朝向来自外部提交的世界空间事实 不读取玩家控制器
    const bool bWantsCrouch = ControlData.bCrouch;
    if (!Context.Execution.bIsMirror)
    {
        // 地面保持完整目标朝向差 视觉转身由 MM 与根骨偏移完成 空中保留原有追向速率
        float Yaw = ControlData.FacingWorld.Yaw;
        if (!Movement.IsMovingOnGround())
        {
            const float YawStep = FMath::Max(Movement.RotationRate.Yaw, 0.0f)
                * FMath::Max(Context.DeltaSeconds, 0.0f);
            Yaw = FMath::FixedTurn(Character.GetActorRotation().Yaw, Yaw, YawStep);
        }
        Character.SetActorRotation(FRotator(0.0f, Yaw, 0.0f));
    }

    if (!Context.Execution.bIsMirror && bWantsCrouch)
    {
        Character.Crouch();
    }

    if (!Context.Execution.bIsMirror && !bWantsCrouch)
    {
        Character.UnCrouch();
    }

    // 跑步只表示站立移动档位 蹲伏由 CMC 独立维护和复制
    if (!Context.Execution.bIsMirror)
    {
        RuntimeData.bRun = ResolveRun(
            Character,
            ControlData,
            Config,
            RuntimeData.bRun,
            Movement.Velocity);
    }

    // 计算并提交当前方向速度映射
    const float DirectionMap = ResolveDirectionMap(
        Character,
        Movement,
        StrafeSpeedMapCurve);

    Movement.MaxWalkSpeed = FMath::Max(
        ResolveMaxSpeed(Config, RuntimeData.bRun, DirectionMap),
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

    // 镜像使用已恢复的移动档位更新共同配置 不重新执行输入或产生移动事实
    if (Context.Execution.bIsMirror)
    {
        return;
    }

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
