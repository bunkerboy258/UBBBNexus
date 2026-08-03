
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAimPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimTypes.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeTypes.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationState.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAimPresentationProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    float DeltaSeconds,
    const FBBBAimConfig &AimConfig,
    const FBBBAimAnimationConfig &AnimationConfig,
    const FBBBAimRuntimeData &AimData,
    FBBBAnimationRuntimeData &AnimationData,
    FBBBCharacterAnimationState &AnimationState) const
{
    const FBBBAimRuntimeState &AimState = AimData.GetState();

    FVector AimOrigin = CharacterMesh.GetComponentLocation() + FVector(0.0f, 0.0f, 50.0f);

    if (!AimConfig.AimOriginSocketName.IsNone()
        && CharacterMesh.DoesSocketExist(AimConfig.AimOriginSocketName))
    {
        AimOrigin = CharacterMesh.GetSocketLocation(AimConfig.AimOriginSocketName);
    }

    const FVector AimTargetWorld = AimState.AimIKTargetWorld;
    const bool bHasValidAimTarget = !AimTargetWorld.IsNearlyZero()
        && !(AimTargetWorld - AimOrigin).IsNearlyZero();

    const FVector RawTarget = CharacterMesh.GetComponentTransform().InverseTransformPosition(
        FVector(AimState.AimIKTargetWorld));
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
    float TargetAimAlpha = 0.0f;
    if (AimState.bIsAiming && bHasValidAimTarget)
    {
        TargetAimAlpha = 1.0f;
    }
    AnimationData.AimPresentation.SmoothedAimIKAlpha = FMath::FInterpTo(
        AnimationData.AimPresentation.SmoothedAimIKAlpha,
        TargetAimAlpha,
        DeltaSeconds,
        AnimationConfig.AimIKAlphaInterpSpeed);
    AnimationState.bWantsAim = AimState.bIsAiming;
    AnimationState.AimTargetComponentSpace = AnimationData.AimPresentation.SmoothedAimTargetComponentSpace;
    AnimationState.bHasValidAimTarget = bHasValidAimTarget;
    AnimationState.AimIKDistanceAlpha = 1.0f;
    if (!AnimationConfig.bEnableNearAimIKDistanceAlpha)
    { return; }
    const float Distance = FVector::Dist(AimOrigin, FVector(AimState.AimIKTargetWorld));
    if (AnimationConfig.SafeAimIKTargetDistance <= AnimationConfig.MinAimIKTargetDistance)
    {
        AnimationState.AimIKDistanceAlpha = 0.0f;
        if (Distance > AnimationConfig.MinAimIKTargetDistance)
        {
            AnimationState.AimIKDistanceAlpha = 1.0f;
        }
        return;
    }
    const float DistanceAlpha = FMath::Clamp(
        (Distance - AnimationConfig.MinAimIKTargetDistance)
            / (AnimationConfig.SafeAimIKTargetDistance - AnimationConfig.MinAimIKTargetDistance),
        0.0f,
        1.0f);
    AnimationState.AimIKDistanceAlpha = DistanceAlpha
        * DistanceAlpha
        * (3.0f - 2.0f * DistanceAlpha);
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
