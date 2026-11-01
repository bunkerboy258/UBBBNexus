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
    State.RecoilTime = 1.0f;
    State.BackwardRecoilStrength = 0.0f;
    State.RecoilAnimation.Reset();
    const auto *Source = Context.AnimationInstance.TryGetWeaponAnimInstance();
    if (!Source)
    {
        return;
    }

    const bool bAiming = Facts.bIsAiming;
    const bool bCrouching = Facts.bIsCrouching;
    const bool bFalling = Facts.MovementMode == MOVE_Falling;
    State.AimFollowSpeed = (bAiming ? Source->GetAimFireAimFollowSpeed() : Source->GetHipFireAimFollowSpeed())
        * (bFalling ? Source->GetAirborneAimFollowScale() : 1.0f)
        * (bCrouching ? Source->GetCrouchedAimFollowScale() : 1.0f);
    if (Source->GetFireSequence() > 0 && !Source->IsReloading())
    {
        State.BackwardRecoilStrength = FMath::Clamp(
            (bAiming ? Source->GetAimFireBackwardRecoilAlpha() : Source->GetHipFireBackwardRecoilAlpha())
            * (bFalling ? Source->GetAirborneBackwardRecoilScale() : 1.0f)
            * (bCrouching ? Source->GetCrouchedBackwardRecoilScale() : 1.0f), 0.0f, 2.0f);
    }
    State.RecoilTime = Source->GetTimeSinceLastFireSeconds();
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
