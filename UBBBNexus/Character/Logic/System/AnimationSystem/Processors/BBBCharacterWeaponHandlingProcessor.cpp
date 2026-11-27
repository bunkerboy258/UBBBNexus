#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterWeaponHandlingProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "Animation/AnimSequence.h"

void FBBBCharacterWeaponHandlingProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    auto &State = Context.RuntimeData.Animation.WeaponHandlingState;
    const auto &Facts = Context.RuntimeData.Animation.ReadAnimationFactState();
    State.PoseOffsetAngles = Context.RuntimeData.Animation.ReadRecoilState().Angles;
    State.AimFollowSpeed = 18.0f;
    State.MinTargetDistance = 120.0f;
    State.RecoilAnimation.Reset();
    auto *Source = Context.AnimationInstance.TryGetWeaponAnimInstance();
    if (!Source)
    {
        State.RecoilSource.Reset();
        State.RecoilFireSequence = 0;
        State.BackwardRecoilStrength = 0.0f;
        return;
    }

    State.MinTargetDistance = Source->GetMinTargetDistance();

    const bool bAiming = Facts.bIsAiming;
    const bool bCrouching = Facts.bIsCrouching;
    const bool bFalling = Facts.MovementMode == MOVE_Falling;
    State.AimFollowSpeed = (bAiming ? Source->GetAimFireAimFollowSpeed() : Source->GetHipFireAimFollowSpeed())
        * (bFalling ? Source->GetAirborneAimFollowScale() : 1.0f)
        * (bCrouching ? Source->GetCrouchedAimFollowScale() : 1.0f);
    constexpr float RecoilHalfLifeSeconds = 0.065f;
    const float DeltaSeconds = FMath::Max(Context.WorldState.FrameDeltaSeconds, 0.0f);
    const int32 FireSequence = Source->GetFireSequence();
    int32 NewShots = 0;
    if (State.RecoilSource.Get() != Source || FireSequence < State.RecoilFireSequence)
    {
        State.RecoilSource = Source;
        State.BackwardRecoilStrength = 0.0f;
    }
    else
    {
        NewShots = FireSequence - State.RecoilFireSequence;
    }
    State.RecoilFireSequence = FireSequence;
    State.BackwardRecoilStrength *= FMath::Exp2(-DeltaSeconds / RecoilHalfLifeSeconds);
    if (NewShots > 0)
    {
        const float Impulse =
            (bAiming ? Source->GetAimFireBackwardRecoilAlpha() : Source->GetHipFireBackwardRecoilAlpha())
            * (bFalling ? Source->GetAirborneBackwardRecoilScale() : 1.0f)
            * (bCrouching ? Source->GetCrouchedBackwardRecoilScale() : 1.0f);
        State.BackwardRecoilStrength = FMath::Clamp(State.BackwardRecoilStrength + 0.5f * Impulse * NewShots, 0.0f, 1.0f);
    }
    if (State.BackwardRecoilStrength < 0.001f)
    {
        State.BackwardRecoilStrength = 0.0f;
    }
    State.RecoilAnimation = Source->GetRecoilAnimation();
    const FVector2D Amplitude = (bAiming ? Source->GetAimFireSwayAmplitudeDegrees() : Source->GetHipFireSwayAmplitudeDegrees())
        * (bFalling ? Source->GetAirborneSwayAmplitudeScale() : 1.0f)
        * (bCrouching ? Source->GetCrouchedSwayAmplitudeScale() : 1.0f);
    const FVector2D Frequency = (bAiming ? Source->GetAimFireSwayFrequency() : Source->GetHipFireSwayFrequency())
        * (bFalling ? Source->GetAirborneSwayFrequencyScale() : 1.0f)
        * (bCrouching ? Source->GetCrouchedSwayFrequencyScale() : 1.0f);
    const FVector2D Phase = Frequency * Source->GetSnapshotTimeSeconds() * (2.0 * UE_PI);
    State.PoseOffsetAngles += FVector2D(FMath::Sin(Phase.X) * Amplitude.X, FMath::Sin(Phase.Y) * Amplitude.Y);
}
