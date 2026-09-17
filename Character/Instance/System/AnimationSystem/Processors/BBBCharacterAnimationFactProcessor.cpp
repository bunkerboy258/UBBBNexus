#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Processors/BBBCharacterAnimationFactProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
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

    if (!bHasSmoothedAimTarget)
    {
        SmoothedAimTargetComponentSpace = RawAimTargetComponentSpace;
        bHasSmoothedAimTarget = true;
    }

    if (AimConfig.bEnableAimIKTargetSmoothing
        && AimConfig.AimIKTargetSmoothTime > 0.0f)
    {
        SmoothedAimTargetComponentSpace = SmoothAimTarget(
            SmoothedAimTargetComponentSpace,
            RawAimTargetComponentSpace,
            AimTargetSmoothVelocity,
            AimConfig.AimIKTargetSmoothTime,
            DeltaSeconds);
    }

    if (!bCanUseAimTarget)
    {
        SmoothedAimTargetComponentSpace = RawAimTargetComponentSpace;
        AimTargetSmoothVelocity = FVector::ZeroVector;
        bHasSmoothedAimTarget = false;
    }

    float GroundDistance = 0.0f;
    if (!Movement->IsMovingOnGround())
    {
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

    if (FVector::DotProduct(Target - Current, Output - Target) > 0.0f)
    {
        Output = Target;
        Velocity = FVector::ZeroVector;
    }

    return Output;
}
