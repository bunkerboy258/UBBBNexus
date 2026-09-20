#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBAnimationState.h"

void FBBBCharacterAnimationActionProcessor::Update(
    UBBBAnimInstance &AnimInstance, FBBBAnimationState &AnimationData) const
{
    for (const FBBBCharacterMontagePlaybackState &Playback : AnimationData.ActiveMontages)
    {
        if (!Playback.Montage || !AnimInstance.Montage_IsPlaying(Playback.Montage))
        {
            AnimationData.Slots.ForEach([&Playback](FBBBCharacterMontageSlotState &Slot)
            {
                if (Slot.Revision == Playback.Revision)
                {
                    Slot.Desired = FBBBCharacterMontageRequestState();
                    Slot.Revision = 0;
                }
            });
            continue;
        }

        if (!AnimationData.Slots.ContainsRevision(Playback.Revision))
        {
            AnimInstance.Montage_Stop(0.1f, Playback.Montage);
        }
    }

    AnimationData.ActiveMontages.RemoveAll([&AnimInstance, &AnimationData](const FBBBCharacterMontagePlaybackState &Playback)
    {
        return !Playback.Montage
            || !AnimInstance.Montage_IsPlaying(Playback.Montage)
            || !AnimationData.Slots.ContainsRevision(Playback.Revision);
    });

    AnimationData.Slots.ForEach([&AnimInstance, &AnimationData](const FBBBCharacterMontageSlotState &Slot)
    {
        if (!Slot.Desired.Montage || Slot.Revision == 0)
        {
            return;
        }
        const bool bPlaying = AnimationData.ActiveMontages.ContainsByPredicate(
            [&Slot](const FBBBCharacterMontagePlaybackState &Playback)
            {
                return Playback.Revision == Slot.Revision;
            });
        if (bPlaying)
        {
            return;
        }
        if (AnimInstance.Montage_Play(Slot.Desired.Montage, Slot.Desired.PlayRate) <= 0.0f)
        {
            return;
        }

        FBBBCharacterMontagePlaybackState &Playback = AnimationData.ActiveMontages.AddDefaulted_GetRef();
        Playback.Montage = Slot.Desired.Montage;
        Playback.Revision = Slot.Revision;
    });
}
