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
    const FBBBCharacterLocomotionConfig &Config,
    const float StaminaCurrent)
{
    if (!ControlData.bRun || StaminaCurrent <= 0.0f)
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
    const FVector &CurrentVelocity,
    const float StaminaCurrent)
{
    const bool bFullMovementInput = ControlData.MoveWorld.Size() >= Config.AnalogRunThreshold;

    if (ControlData.bAim || ControlData.bCrouch || StaminaCurrent <= 0.0f)
    {
        return false;
    }

    if (ControlData.MoveWorld.IsNearlyZero()
        && CurrentVelocity.SizeSquared2D() > FMath::Square(1.0f)
        && bWasRunning)
    {
        return true;
    }

    return bFullMovementInput && CanRun(Character, ControlData, Config, StaminaCurrent);
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

// 坡度相对滑行方向的有符号角度 下坡为正
float ResolveSlideSlopeDegrees(const UCharacterMovementComponent &Movement)
{
    if (!Movement.CurrentFloor.bBlockingHit)
    {
        return 0.0f;
    }

    const FVector Normal = Movement.CurrentFloor.HitResult.ImpactNormal.GetSafeNormal();
    if (Normal.IsNearlyZero())
    {
        return 0.0f;
    }

    const float UpDot = FMath::Clamp(FVector::DotProduct(Normal, FVector::UpVector), -1.0f, 1.0f);
    const float SlopeDegrees = FMath::RadiansToDegrees(FMath::Acos(UpDot));
    const FVector DownHill = FVector::VectorPlaneProject(FVector::DownVector, Normal).GetSafeNormal();
    if (DownHill.IsNearlyZero())
    {
        return 0.0f;
    }

    return SlopeDegrees * FVector::DotProduct(Movement.Velocity.GetSafeNormal2D(), DownHill);
}

float ResolveSlideMaxSpeed(
    const FBBBCharacterLocomotionConfig &Config,
    const float SignedSlopeDegrees)
{
    if (SignedSlopeDegrees <= Config.SlideShallowSlopeAngle)
    {
        return Config.SlideFlatGroundSpeed;
    }

    const float DownAlpha = FMath::Clamp(
        FMath::GetRangePct(Config.SlideShallowSlopeAngle, Config.SlideSteepSlopeAngle, SignedSlopeDegrees),
        0.0f,
        1.0f);
    return FMath::Lerp(Config.SlideShallowSlopeSpeed, Config.SlideSteepSlopeSpeed, DownAlpha);
}

float ResolveSlideDeceleration(
    const FBBBCharacterLocomotionConfig &Config,
    const float SignedSlopeDegrees)
{
    const float Uphill = -SignedSlopeDegrees;
    if (Uphill <= Config.SlideShallowSlopeAngle)
    {
        return Config.SlideFlatGroundDeceleration;
    }

    const float UpAlpha = FMath::Clamp(
        FMath::GetRangePct(Config.SlideShallowSlopeAngle, Config.SlideSteepSlopeAngle, Uphill),
        0.0f,
        1.0f);
    return FMath::Lerp(Config.SlideFlatGroundDeceleration, Config.SlideSteepSlopeDeceleration, UpAlpha);
}

void UpdateSlideState(
    FBBBCharacterLocomotionState &RuntimeData,
    const UCharacterMovementComponent &Movement,
    const bool bCrouchPressed,
    const FBBBCharacterLocomotionConfig &Config,
    const float DeltaSeconds,
    const float StaminaCurrent)
{
    const float Speed = Movement.Velocity.Size2D();
    const bool bOnGround = Movement.IsMovingOnGround();
    if (RuntimeData.bSlide)
    {
        const bool bBelowExitSpeed = Speed < Config.SlideExitSpeed;
        if (!bOnGround || (bBelowExitSpeed && RuntimeData.bSlideBelowExitSpeed))
        {
            RuntimeData.bSlide = false;
            RuntimeData.SlideElapsed = 0.0f;
            RuntimeData.bSlideBelowExitSpeed = false;
        }
        else
        {
            RuntimeData.SlideElapsed += FMath::Max(DeltaSeconds, 0.0f);
            RuntimeData.bSlideBelowExitSpeed = bBelowExitSpeed;
        }
        return;
    }

    if (bOnGround && bCrouchPressed && Speed >= Config.SlideEnterSpeed && StaminaCurrent > 0.0f)
    {
        RuntimeData.bSlide = true;
        RuntimeData.bSlideBelowExitSpeed = false;
        RuntimeData.SlideElapsed = 0.0f;
    }
}

void ApplySlideMovement(
    ACharacter &Character,
    UCharacterMovementComponent &Movement,
    FBBBCharacterLocomotionState &RuntimeData,
    const FBBBCharacterControlState &ControlData,
    const FBBBCharacterLocomotionConfig &Config,
    const float SpeedScale,
    const bool bMirror,
    const float DeltaSeconds)
{
    const float SignedSlope = ResolveSlideSlopeDegrees(Movement);
    float MaxSpeed = ResolveSlideMaxSpeed(Config, SignedSlope);
    const bool bBoost = !bMirror && RuntimeData.SlideElapsed < Config.SlideInitialBoostTime;
    if (bBoost)
    {
        MaxSpeed = FMath::Max(MaxSpeed, Config.SlideInitialBoostSpeed);
    }

    MaxSpeed = FMath::Max(MaxSpeed * FMath::Max(SpeedScale, 0.0f), 1.0f);
    Movement.MaxWalkSpeed = MaxSpeed;
    Movement.MaxWalkSpeedCrouched = MaxSpeed;
    Movement.GroundFriction = FMath::Max(Config.SlideGroundFriction, 0.0f);
    Movement.BrakingFriction = 0.0f;
    Movement.BrakingFrictionFactor = 0.0f;
    Movement.bUseSeparateBrakingFriction = false;
    Movement.BrakingDecelerationWalking = FMath::Max(ResolveSlideDeceleration(Config, SignedSlope), 0.0f);
    Movement.MaxAcceleration = 0.0f;
    if (bBoost)
    {
        Movement.MaxAcceleration = FMath::Max(Config.SlideInitialBoostAcceleration, 0.0f);
    }

    if (bMirror)
    {
        return;
    }

    const bool bHasInput = !ControlData.MoveWorld.IsNearlyZero();
    // 平地和上坡在初始加速后进入制动 下坡仍允许输入沿滑行方向加速
    const bool bDriveDownhill = bHasInput && SignedSlope > Config.SlideShallowSlopeAngle;
    if (!bBoost && bDriveDownhill)
    {
        Movement.MaxAcceleration = FMath::Max(Config.SlideAfterBoostAcceleration, 0.0f);
    }

    FVector Drive = Movement.Velocity.GetSafeNormal2D();
    const FVector Wish = ControlData.MoveWorld.GetSafeNormal2D();
    if (!Drive.IsNearlyZero() && !Wish.IsNearlyZero())
    {
        const float DeltaYaw = FMath::FindDeltaAngleDegrees(Drive.Rotation().Yaw, Wish.Rotation().Yaw);
        const float Yaw = Drive.Rotation().Yaw
            + DeltaYaw * FMath::Max(Config.SlideTurningStrength, 0.0f) * FMath::Max(DeltaSeconds, 0.0f);
        const FVector Turned = FRotator(0.0f, Yaw, 0.0f).Vector();
        const float PlanarSpeed = Movement.Velocity.Size2D();
        Movement.Velocity = FVector(Turned.X * PlanarSpeed, Turned.Y * PlanarSpeed, Movement.Velocity.Z);
        Drive = Turned;
    }

    if (!Drive.IsNearlyZero() && (bBoost || bDriveDownhill))
    {
        const float Scale = bBoost ? 1.0f : ControlData.MoveWorld.Size();
        Character.AddMovementInput(Drive, Scale);
    }

    if (!Movement.Velocity.GetSafeNormal2D().IsNearlyZero())
    {
        Character.SetActorRotation(FRotator(0.0f, Movement.Velocity.Rotation().Yaw, 0.0f));
    }
}

}

void FBBBCharacterLocomotionProcessor::Update(
    FBBBCharacterLocomotionUpdateContext &Context) const
{
    ACharacter &Character = Context.Character;
    UCharacterMovementComponent &Movement = Context.Movement;
    FBBBCharacterLocomotionState &RuntimeData = Context.LocomotionState;
    const FBBBCharacterControlState &ControlData = Context.ControlState;
    const bool bIsLocal = !Context.Execution.bIsMirror;
    const bool bCrouchPressed = bIsLocal && ControlData.bCrouch && !RuntimeData.bWasCrouchRequested;
    if (bIsLocal)
    {
        RuntimeData.bWasCrouchRequested = ControlData.bCrouch;
    }

    if (Context.Data.PhysicalPresentation.ReadPhysicalPresentationState().bRagdoll)
    {
        Character.StopJumping();
        Character.ConsumeMovementInputVector();
        Movement.StopMovementImmediately();
        Movement.ClearAccumulatedForces();
        Movement.CurrentRootMotion.Clear();
        Movement.RootMotionParams.Clear();
        Movement.DisableMovement();
        Movement.bUseControllerDesiredRotation = false;
        Movement.NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;
        Character.GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        RuntimeData.bRun = false;
        RuntimeData.bSlide = false;
        RuntimeData.SlideElapsed = 0.0f;
        return;
    }
    if (Context.Data.PhysicalPresentation.ReadPhysicalPresentationState().RagdollSnapshot.Phase
            == EBBBCharacterPhysicalPhase::Recovering || !Context.Life.bActionsAllowed)
    {
        if (RuntimeData.bSlide)
        {
            RuntimeData.bSlide = false;
            RuntimeData.SlideElapsed = 0.0f;
            Movement.GroundFriction = FMath::Max(Context.Config.GroundFriction, 0.0f);
            Movement.BrakingDecelerationWalking = FMath::Max(Context.Config.BrakingDeceleration, 0.0f);
            Movement.BrakingFriction = FMath::Max(Context.Config.BrakingFriction, 0.0f);
        }
        return;
    }
    const FBBBCharacterLocomotionConfig &Config = Context.Config;
    const UCurveFloat &StrafeSpeedMapCurve = Context.StrafeSpeedMapCurve;
    // 镜像使用 CMC 已接收的控制朝向还原旋转 地面保持即时到达 空中保留追向速率
    Movement.bUseControllerDesiredRotation = Context.Execution.bIsMirror;
    Movement.RotationRate.Yaw = 360.0f;
    if (Context.Execution.bIsMirror && Movement.IsMovingOnGround())
    {
        Movement.RotationRate.Yaw = -1.0f;
    }

    const float StaminaCurrent = Context.Data.Stamina.ReadStaminaState().Current;
    if (bIsLocal)
    {
        // 使用蹲伏与普通步态限速生效之前的水平速度裁决滑铲
        UpdateSlideState(
            RuntimeData, Movement, bCrouchPressed, Config, Context.DeltaSeconds, StaminaCurrent);
        // 滑铲期间本机暂停生成站立跑步状态
        if (RuntimeData.bSlide)
        {
            RuntimeData.bRun = false;
        }
    }

    const bool bWantsCrouch = ControlData.bCrouch || RuntimeData.bSlide;
    if (bIsLocal && bWantsCrouch)
    {
        Character.Crouch();
    }
    if (bIsLocal && !bWantsCrouch)
    {
        Character.UnCrouch();
    }

    if (RuntimeData.bSlide)
    {
        ApplySlideMovement(
            Character,
            Movement,
            RuntimeData,
            ControlData,
            Config,
            Context.Life.MovementSpeedScale,
            Context.Execution.bIsMirror,
            Context.DeltaSeconds);
        return;
    }

    // 朝向来自外部提交的世界空间事实 不读取玩家控制器
    if (bIsLocal)
    {
        // NOTE: 重大技术路线 BUG
        // 之前这里加了 360°/秒限速 让胶囊本身也慢慢转过去
        // 因为这条限速代码，改变了 MM 判断“现在需要怎样转身”的依据
        // 导致“大转身没有及时出现，身体一直用小碎步蹭过去”的表现问题
        float Yaw = ControlData.FacingWorld.Yaw;
        if (!Movement.IsMovingOnGround())
        {
            const float YawStep = FMath::Max(Movement.RotationRate.Yaw, 0.0f)
                * FMath::Max(Context.DeltaSeconds, 0.0f);
            Yaw = FMath::FixedTurn(Character.GetActorRotation().Yaw, Yaw, YawStep);
        }
        Character.SetActorRotation(FRotator(0.0f, Yaw, 0.0f));
        // 跑步只表示站立移动档位 蹲伏由 CMC 独立维护和复制
        RuntimeData.bRun = ResolveRun(
            Character,
            ControlData,
            Config,
            RuntimeData.bRun,
            Movement.Velocity,
            StaminaCurrent);
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

    // 每帧从基础速度应用当前结果 避免持续相乘累积减速
    Movement.MaxWalkSpeed *= Context.Life.MovementSpeedScale;
    Movement.MaxWalkSpeedCrouched *= Context.Life.MovementSpeedScale;

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
