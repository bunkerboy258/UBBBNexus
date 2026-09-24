#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/DomainData/States/BBBAimState.h"

void FBBBCharacterAimStateProcessor::Update(
    const FBBBCharacterControlState &ControlData,
    FBBBAimState &State) const
{
    State.bIsAiming = ControlData.bAim;
}

void FBBBCharacterAimStateProcessor::InterpolateAimAlpha(
    FBBBAimState &State,
    const bool bCanAim,
    const float DeltaSeconds,
    const float InterpSpeed) const
{
    const float TargetAimAlpha = bCanAim && State.bIsAiming ? 1.0f : 0.0f;
    State.AimAlpha = FMath::Clamp(
        FMath::FInterpTo(
            State.AimAlpha,
            TargetAimAlpha,
            DeltaSeconds,
            FMath::Max(InterpSpeed, 0.0f)),
        0.0f,
        1.0f);
}
