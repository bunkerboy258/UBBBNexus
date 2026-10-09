#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterLocomotionProcessor.h"

#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "Curves/CurveFloat.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBTraversalNetworkObservationState.h"

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
    const float DirectionDelta = FMath::Abs(FMath::FindDeltaAngleDegrees(
        Character.GetActorRotation().Yaw,
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
    const bool bSuppressCorrection = Context.Data.Network.ReadTraversalNetworkObservationState()
        .bMovementCorrectionSuppressed;
    if (bSuppressCorrection && !RuntimeData.bTraversalCorrectionOverride)
    {
        // 网络领域决定策略 移动领域集中应用并保留接管前的组件设置
        RuntimeData.bSavedIgnoreMovementError = Movement.bIgnoreClientMovementErrorChecksAndCorrection;
        RuntimeData.bSavedIgnoreMovementCorrection = Movement.bClientIgnoreMovementCorrections;
        RuntimeData.bSavedAcceptClientPosition = Movement.bServerAcceptClientAuthoritativePosition;
        Movement.bIgnoreClientMovementErrorChecksAndCorrection = true;
        Movement.bClientIgnoreMovementCorrections = true;
        Movement.bServerAcceptClientAuthoritativePosition = true;
        RuntimeData.bTraversalCorrectionOverride = true;
    }
    if (!bSuppressCorrection && RuntimeData.bTraversalCorrectionOverride)
    {
        Movement.bIgnoreClientMovementErrorChecksAndCorrection = RuntimeData.bSavedIgnoreMovementError;
        Movement.bClientIgnoreMovementCorrections = RuntimeData.bSavedIgnoreMovementCorrection;
        Movement.bServerAcceptClientAuthoritativePosition = RuntimeData.bSavedAcceptClientPosition;
        RuntimeData.bTraversalCorrectionOverride = false;
    }
    if (!Context.Life.bActionsAllowed)
    {
        return;
    }
    const FBBBCharacterControlState &ControlData = Context.ControlState;
    const FBBBCharacterLocomotionConfig &Config = Context.Config;
    const UCurveFloat &StrafeSpeedMapCurve = Context.StrafeSpeedMapCurve;
    const FBBBCharacterTraversalState &Traversal = Context.Traversal;
    const bool bWantsTraversalControl = Traversal.Action != EBBBTraversalAction::None
        && Traversal.bPlaybackRequested
        && !(Traversal.bEndRequested && Traversal.bAnimationReleased);

    FFindFloorResult ExitFloor;
    if (RuntimeData.bTraversalControlled && Traversal.bEndRequested)
    {
        Movement.ComputeFloorDist(Character.GetActorLocation(), 8.0f, 8.0f, ExitFloor,
            Character.GetCapsuleComponent()->GetScaledCapsuleRadius());
    }

    // 结束裁决与交权速度同帧成立 在关闭根运动前发布供各端还原
    if (RuntimeData.bTraversalControlled && Traversal.bEndRequested
        && !Context.Execution.bIsMirror && !RuntimeData.bTraversalExitPrepared)
    {
        /** 校正位移不作为额外加速来源 退出速度受当前移动档位约束 */
        const float ExitSpeed = FMath::Min(
            FMath::Max(RuntimeData.TraversalEntrySpeed, Movement.Velocity.Size2D()),
            Movement.MaxWalkSpeed);
        // 校正动画不决定接管后的方向 无输入立即消除尾速 有输入沿已解析的世界方向交接
        RuntimeData.TraversalExitVelocity = ControlData.MoveWorld.GetSafeNormal2D() * ExitSpeed;

        // 空中交接保留升降趋势 不让二维移动意图抹掉正在发生的下落
        if (!ExitFloor.IsWalkableFloor())
        {
            const float FeetZ = Character.GetActorLocation().Z
                - Character.GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
            const float FallDistance = FMath::Max(FeetZ - Traversal.EndTarget.GetLocation().Z,
                Character.GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
            const float Gravity = FMath::Abs(Movement.GetGravityZ());
            float VerticalSpeed = Movement.Velocity.Z;
            // 剩余落差对应的自由落体速度限制校正尖峰 胶囊高度避免近地交接再次趋零
            if (Gravity > UE_KINDA_SMALL_NUMBER)
            {
                const float SpeedLimit = FMath::Sqrt(2.0f * Gravity * FallDistance);
                VerticalSpeed = FMath::Clamp(VerticalSpeed, -SpeedLimit, SpeedLimit);
            }
            RuntimeData.TraversalExitVelocity.Z = VerticalSpeed;
        }
        RuntimeData.bTraversalExitPrepared = true;
    }

    // 镜像直接执行接受的结束结果 控制方等待自身根运动释放后交接
    if (RuntimeData.bTraversalControlled && !bWantsTraversalControl)
    {
        Movement.Velocity = RuntimeData.TraversalExitVelocity;
        Movement.SetMovementMode(ExitFloor.IsWalkableFloor() ? MOVE_Walking : MOVE_Falling);
        RuntimeData.bTraversalControlled = false;
        RuntimeData.TraversalEntrySpeed = 0.0f;
    }

    if (bWantsTraversalControl && !RuntimeData.bTraversalControlled)
    {
        Character.StopJumping();
        Character.ConsumeMovementInputVector();
        RuntimeData.TraversalEntrySpeed = Movement.Velocity.Size2D();
        RuntimeData.TraversalExitVelocity = FVector::ZeroVector;
        RuntimeData.bTraversalExitPrepared = false;
        Movement.SetMovementMode(MOVE_Flying);
        RuntimeData.bTraversalControlled = true;
    }

    if (Context.Execution.bIsMirror)
    {
        return;
    }

    if (Traversal.Action != EBBBTraversalAction::None && !Traversal.bAnimationReleased)
    {
        return;
    }
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

    // 跑步只表示站立移动档位 蹲伏由 CMC 独立维护和复制
    RuntimeData.bRun = ResolveRun(
        Character,
        ControlData,
        Config,
        RuntimeData.bRun,
        Movement.Velocity);

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
