#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Processors/BBBCharacterAnimationFactProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
constexpr float GroundTraceDistance = 100000.0f;
}

void FBBBCharacterAnimationFactProcessor::Update(
    ABBBCharacter &Character,
    FBBBCharacterRuntimeData &RuntimeData,
    FBBBCharacterAnimationFacts &OutFacts,
    float DeltaSeconds)
{
    // 采集动画事实前确认角色组件和世界对象有效
    UCharacterMovementComponent *Movement = Character.GetCharacterMovement();
    USkeletalMeshComponent *CharacterMesh = Character.GetMesh();
    UWorld *World = Character.GetWorld();
    UCapsuleComponent *Capsule = Character.GetCapsuleComponent();
    if (!ensureMsgf(
        Movement && CharacterMesh && World && Capsule,
        TEXT("[UBBBC]Animation fact capture failed because engine dependencies are null")))
    {
        return;
    }

    const FBBBAimRuntimeState &AimState = RuntimeData.Aim.GetState();
    const FBBBCharacterEquipmentState &EquipmentState = RuntimeData.Equipment.Equipment;
    const FBBBAimAnimationConfig &AimConfig = Character.GetCharacterConfig().AimAnimation;
    const bool bHasActiveMainHandEquipment = EquipmentState.GetActiveMainHandInstance() != nullptr;

    // 优先使用配置骨骼作为瞄准起点否则使用角色网格位置
    FVector AimOrigin = CharacterMesh->GetComponentLocation() + FVector(0.0f, 0.0f, 50.0f);
    if (!AimConfig.AimIKOriginBoneName.IsNone()
        && CharacterMesh->DoesSocketExist(AimConfig.AimIKOriginBoneName))
    {
        AimOrigin = CharacterMesh->GetSocketLocation(AimConfig.AimIKOriginBoneName);
    }

    const FVector AimTargetWorld = AimState.AimTargetWorld;
    const bool bCanUseAimTarget = !AimTargetWorld.ContainsNaN()
        && !AimOrigin.ContainsNaN()
        && !(AimTargetWorld - AimOrigin).IsNearlyZero();
    const FVector RawAimTargetComponentSpace = bCanUseAimTarget
        ? CharacterMesh->GetComponentTransform().InverseTransformPosition(AimTargetWorld)
        : FVector::ZeroVector;

    // 首次采集时直接建立平滑目标的初始值
    if (!bHasSmoothedAimTarget)
    {
        SmoothedAimTargetComponentSpace = RawAimTargetComponentSpace;
        bHasSmoothedAimTarget = true;
    }

    if (AimConfig.bEnableAimIKTargetSmoothing
        && AimConfig.AimIKTargetSmoothTime > 0.0f)
    {
        // 按配置时间平滑瞄准目标避免目标点瞬移
        SmoothedAimTargetComponentSpace = SmoothAimTarget(
            SmoothedAimTargetComponentSpace,
            RawAimTargetComponentSpace,
            AimTargetSmoothVelocity,
            AimConfig.AimIKTargetSmoothTime,
            DeltaSeconds);
    }

    if (!bCanUseAimTarget)
    {
        // 目标无效时清除平滑速度并等待下一次有效目标
        SmoothedAimTargetComponentSpace = RawAimTargetComponentSpace;
        AimTargetSmoothVelocity = FVector::ZeroVector;
        bHasSmoothedAimTarget = false;
    }

    float GroundDistance = 0.0f;
    if (!Movement->IsMovingOnGround())
    {
        // 空中状态向下追踪地面距离供动画判断下落高度
        const FVector TraceStart = Character.GetActorLocation();
        const FVector TraceEnd = TraceStart - FVector(
            0.0f,
            0.0f,
            GroundTraceDistance + Capsule->GetScaledCapsuleHalfHeight());
        FCollisionQueryParams QueryParams(
            SCENE_QUERY_STAT(BBBAnimationGroundDistance),
            false,
            &Character);
        FHitResult Hit;

        GroundDistance = -1.0f;
        if (World->LineTraceSingleByChannel(
            Hit,
            TraceStart,
            TraceEnd,
            ECC_Visibility,
            QueryParams))
        {
            GroundDistance = FMath::Max(
                Hit.Distance - Capsule->GetScaledCapsuleHalfHeight(),
                0.0f);
        }
    }

    const float TargetAimIntentAlpha = AimState.bIsAiming ? 1.0f : 0.0f;

    // 平滑瞄准意图权重供动画层渐进过渡
    SmoothedAimIntentAlpha = FMath::FInterpTo(
        SmoothedAimIntentAlpha,
        TargetAimIntentAlpha,
        DeltaSeconds,
        AimConfig.AimIntentAlphaInterpSpeed);

    UBBBAnimInstance *CharacterAnim = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
    UBBBEquipmentAnimInstance *WeaponAnim = CharacterAnim ? CharacterAnim->TryGetWeaponAnimInstance() : nullptr;
    OutFacts.bIsAiming = AimState.bIsAiming;
    OutFacts.AimIntentAlpha = FMath::Clamp(SmoothedAimIntentAlpha, 0.0f, 1.0f);
    OutFacts.AimIKAlpha = 0.0f;
    OutFacts.AimTargetComponentSpace = SmoothedAimTargetComponentSpace;
    // 只有装备和瞄准来源都有效时才启用瞄准逆向运动学
    if (bHasActiveMainHandEquipment
        && WeaponAnim
        && bCanUseAimTarget
        && WeaponAnim->HasValidAimSource()
        && WeaponAnim->GetAimSourceLocalTransform().IsValid())
    {
        OutFacts.AimIKAlpha = OutFacts.AimIntentAlpha;
    }

    OutFacts.ActorLocation = Character.GetActorLocation();
    OutFacts.ActorRotation = Character.GetActorRotation();
    OutFacts.Velocity = Movement->Velocity;
    OutFacts.LastUpdateVelocity = Movement->GetLastUpdateVelocity();
    OutFacts.Acceleration = Movement->GetCurrentAcceleration();
    OutFacts.Gait = RuntimeData.Locomotion.GetGait();
    OutFacts.MovementMode = Movement->MovementMode;
    OutFacts.GroundFriction = Movement->GroundFriction;
    OutFacts.BrakingFriction = Movement->BrakingFriction;
    OutFacts.BrakingFrictionFactor = Movement->BrakingFrictionFactor;
    OutFacts.BrakingDecelerationWalking = Movement->BrakingDecelerationWalking;
    OutFacts.GravityZ = Movement->GetGravityZ();
    OutFacts.GroundDistance = GroundDistance;
    OutFacts.bUseSeparateBrakingFriction = Movement->bUseSeparateBrakingFriction;
    OutFacts.bIsMovingOnGround = Movement->IsMovingOnGround();
    OutFacts.bIsCrouching = Movement->IsCrouching();
}

//------------------------------------------------------------------------------

FVector FBBBCharacterAnimationFactProcessor::SmoothAimTarget(
    const FVector &Current,
    const FVector &Target,
    FVector &Velocity,
    float SmoothTime,
    float DeltaSeconds) const
{
    // 没有有效帧间隔时保持当前平滑结果
    if (DeltaSeconds <= 0.0f)
    {
        return Current;
    }

    const float SafeSmoothTime = FMath::Max(0.0001f, SmoothTime);
    const float Omega = 2.0f / SafeSmoothTime;
    const float X = Omega * DeltaSeconds;
    const float Exp = 1.0f / (1.0f + X + 0.48f * X * X + 0.235f * X * X * X);
    const FVector Change = Current - Target;
    const FVector Temp = (Velocity + Omega * Change) * DeltaSeconds;
    Velocity = (Velocity - Omega * Temp) * Exp;
    FVector Output = Target + (Change + Temp) * Exp;

    // 越过目标时直接收敛并清除剩余速度
    if (FVector::DotProduct(Target - Current, Output - Target) > 0.0f)
    {
        Output = Target;
        Velocity = FVector::ZeroVector;
    }

    return Output;
}
