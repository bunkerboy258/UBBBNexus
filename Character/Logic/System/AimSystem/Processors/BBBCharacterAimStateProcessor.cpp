#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/Processors/BBBCharacterAimStateProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AimSystem/DomainData/States/BBBAimState.h"

void FBBBCharacterAimStateProcessor::Update(
    const FBBBCharacterControlState &ControlData,
    FBBBAimState &State) const
{
    // 瞄准或开火意图存在时都保持瞄准状态
    State.bIsAiming = ControlData.bAim || ControlData.bFire;
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
