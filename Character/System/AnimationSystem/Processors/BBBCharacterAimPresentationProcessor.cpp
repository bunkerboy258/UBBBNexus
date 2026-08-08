
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAimPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAimPresentationProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    float DeltaSeconds,
    const FBBBAimAnimationConfig &AnimationConfig,
    const FBBBAimRuntimeData &AimData,
    const FBBBFacingRuntimeData &FacingData,
    FBBBAnimationRuntimeData &AnimationData,
    FBBBCharacterAnimationState &AnimationState) const
{
    const FBBBAimRuntimeState &AimState = AimData.GetState();

    const FVector AimOrigin = FacingData.GetAimOriginWorld();
    const FVector AimTargetWorld = AimState.AimTargetWorld;
    const bool bHasValidAimTarget = !AimTargetWorld.IsNearlyZero()
        && !(AimTargetWorld - AimOrigin).IsNearlyZero();

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
    AnimationState.AimTargetComponentSpace = AnimationData.AimPresentation.SmoothedAimTargetComponentSpace;
    AnimationState.bHasValidAimTarget = bHasValidAimTarget;
    AnimationState.AimIKDistanceAlpha = 1.0f;
    if (!AnimationConfig.bEnableNearAimIKDistanceAlpha)
    { return; }
    const float Distance = FVector::Dist(AimOrigin, FVector(AimState.AimTargetWorld));
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
