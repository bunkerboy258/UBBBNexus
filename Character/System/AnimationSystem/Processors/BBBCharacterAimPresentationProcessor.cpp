
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAimPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

void FBBBCharacterAimPresentationProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    float DeltaSeconds,
    const FBBBAimAnimationConfig &AnimationConfig,
    const FBBBAimRuntimeData &AimData,
    const FBBBCharacterAnimationCommands &AnimationCommands,
    FBBBAnimationRuntimeData &AnimationData,
    FBBBCharacterAnimationState &AnimationState) const
{
    const AActor *Owner = CharacterMesh.GetOwner();
    if (!ensureMsgf(Owner, TEXT("[UBBBC]Aim presentation update failed because mesh owner is null")))
    {
        return;
    }

    const FBBBAimRuntimeState &AimState = AimData.GetState();

    FVector AimOrigin = CharacterMesh.GetComponentLocation() + FVector(0.0f, 0.0f, 50.0f);
    if (!AnimationConfig.AimIKOriginBoneName.IsNone()
        && CharacterMesh.DoesSocketExist(AnimationConfig.AimIKOriginBoneName))
    {
        AimOrigin = CharacterMesh.GetSocketLocation(AnimationConfig.AimIKOriginBoneName);
    }

    const FVector AimTargetWorld = AimState.AimTargetWorld;
    const bool bHasValidAimTarget = !AimTargetWorld.IsNearlyZero()
        && !(AimTargetWorld - AimOrigin).IsNearlyZero();

    float TargetAimOffsetYaw = 0.0f;
    if (bHasValidAimTarget)
    {
        const FMatrix ReferenceMatrix = FRotationMatrix::MakeFromXZ(
            Owner->GetActorForwardVector(),
            Owner->GetActorUpVector());
        const FVector AimDirection = (AimTargetWorld - AimOrigin).GetSafeNormal();
        const FVector LocalAimDirection = ReferenceMatrix.InverseTransformVector(AimDirection);
        TargetAimOffsetYaw = FMath::Clamp(LocalAimDirection.Rotation().Yaw, -90.0f, 90.0f);
    }

    AnimationData.AimPresentation.SmoothedAimOffsetYaw = FMath::FInterpTo(
        AnimationData.AimPresentation.SmoothedAimOffsetYaw,
        TargetAimOffsetYaw,
        DeltaSeconds,
        AnimationConfig.AimOffsetYawInterpSpeed);
    AnimationState.AimOffsetYaw = AnimationData.AimPresentation.SmoothedAimOffsetYaw;

    const FVector RawTarget = CharacterMesh.GetComponentTransform().InverseTransformPosition(
        FVector(AimState.AimTargetWorld));
    if (!AnimationData.AimPresentation.bHasSmoothedAimTarget)
    {
        AnimationData.AimPresentation.SmoothedAimTargetComponentSpace = RawTarget;
        AnimationData.AimPresentation.bHasSmoothedAimTarget = true;
    }
    if (AnimationConfig.bEnableAimIKTargetSmoothing
        && AnimationConfig.AimIKTargetSmoothTime > 0.0f)
    {
        AnimationData.AimPresentation.SmoothedAimTargetComponentSpace = SmoothTarget(
            AnimationData.AimPresentation.SmoothedAimTargetComponentSpace,
            RawTarget,
            AnimationData.AimPresentation.AimTargetSmoothVelocity,
            AnimationConfig.AimIKTargetSmoothTime,
            DeltaSeconds);
    }
    if (!bHasValidAimTarget)
    {
        AnimationData.AimPresentation.SmoothedAimTargetComponentSpace = RawTarget;
        AnimationData.AimPresentation.AimTargetSmoothVelocity = FVector::ZeroVector;
        AnimationData.AimPresentation.bHasSmoothedAimTarget = false;
    }
    float TargetAimIntentAlpha = 0.0f;
    if (AimState.bIsAiming)
    {
        TargetAimIntentAlpha = 1.0f;
    }

    AnimationData.AimPresentation.SmoothedAimIntentAlpha = FMath::FInterpTo(
        AnimationData.AimPresentation.SmoothedAimIntentAlpha,
        TargetAimIntentAlpha,
        DeltaSeconds,
        AnimationConfig.AimIntentAlphaInterpSpeed);

    float TargetAimIKLockAlpha = 1.0f;
    if (AnimationCommands.IsAimIKBlockedRequested())
    {
        TargetAimIKLockAlpha = 0.0f;
    }

    AnimationData.AimPresentation.SmoothedAimIKLockAlpha = FMath::FInterpTo(
        AnimationData.AimPresentation.SmoothedAimIKLockAlpha,
        TargetAimIKLockAlpha,
        DeltaSeconds,
        AnimationConfig.AimIKLockAlphaInterpSpeed);

    AnimationState.AimIntentAlpha = FMath::Clamp(
        AnimationData.AimPresentation.SmoothedAimIntentAlpha,
        0.0f,
        1.0f);
    const float AimIKLockAlpha = FMath::Clamp(
        AnimationData.AimPresentation.SmoothedAimIKLockAlpha,
        0.0f,
        1.0f);
    AnimationState.AimIKAlpha = AnimationState.AimIntentAlpha * AimIKLockAlpha;
    AnimationState.AimTargetComponentSpace = AnimationData.AimPresentation.SmoothedAimTargetComponentSpace;
    AnimationState.bHasValidAimTarget = bHasValidAimTarget;
}

FVector FBBBCharacterAimPresentationProcessor::SmoothTarget(
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
