#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/BBBEquipmentAnimInstance.h"

void UBBBEquipmentAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (UWorld* World = GetWorld())
    {
        AnimationState.CurrentWorldTimeSeconds = World->GetTimeSeconds();
    }

    AnimationState.FireElapsedSeconds = FMath::Max(
        AnimationState.CurrentWorldTimeSeconds - AnimationState.LastFireTimeSeconds,
        0.0f
    );
    AnimationState.ReloadElapsedSeconds = FMath::Max(
        AnimationState.CurrentWorldTimeSeconds - AnimationState.ReloadStartTimeSeconds,
        0.0f
    );
    AnimationState.bFireActive = AnimationState.FireSequence > 0 && AnimationState.FireElapsedSeconds < 0.333333f;
}

void UBBBEquipmentAnimInstance::NotifyFire(const float WorldTimeSeconds)
{
    AnimationState.FireSequence++;
    AnimationState.LastFireTimeSeconds = WorldTimeSeconds;
    AnimationState.bFireActive = true;
}

void UBBBEquipmentAnimInstance::NotifyReloadStarted(
    const float WorldTimeSeconds,
    const float DurationSeconds)
{
    AnimationState.ReloadSequence++;
    AnimationState.bIsReloading = true;
    AnimationState.ReloadStartTimeSeconds = WorldTimeSeconds;
    AnimationState.ReloadDurationSeconds = FMath::Max(DurationSeconds, 0.01f);
}

void UBBBEquipmentAnimInstance::NotifyReloadCompleted(const float WorldTimeSeconds)
{
    AnimationState.bIsReloading = false;
}
