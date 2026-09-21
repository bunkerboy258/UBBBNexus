#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterAnimationState.h"

namespace
{
    template<typename TSlots, typename TFunction>
    void ForEachSlot(TSlots &Slots, TFunction &&Function)
    {
        Function(Slots.FullBody);
        Function(Slots.UpperBody);
        Function(Slots.FullBodyAdditivePreAim);
        Function(Slots.UpperBodyAdditive);
        Function(Slots.AdditiveHitReact);
    }

    bool ContainsRevision(
        const FBBBCharacterMontageSlots &Slots,
        const uint64 Revision)
    {
        bool bFound = false;
        ForEachSlot(Slots, [Revision, &bFound](const FBBBCharacterMontageSlot &Slot)
        {
            bFound |= Slot.Revision == Revision;
        });
        return bFound;
    }
}

void FBBBCharacterAnimationActionProcessor::Update(
    FBBBCharacterAnimationUpdateContext &Context) const
{
    UBBBAnimInstance &AnimInstance = Context.AnimationInstance;
    FBBBCharacterAnimationState &AnimationData = Context.AnimationState;
    for (const FBBBCharacterMontagePlaybackEntry &Playback : AnimationData.ActiveMontages)
    {
        if (!Playback.Montage || !AnimInstance.Montage_IsPlaying(Playback.Montage))
        {
            ForEachSlot(AnimationData.Slots, [&Playback](FBBBCharacterMontageSlot &Slot)
            {
                if (Slot.Revision == Playback.Revision)
                {
                    Slot.Desired = FBBBCharacterMontageRequest();
                    Slot.Revision = 0;
                }
            });
            continue;
        }

        if (!ContainsRevision(AnimationData.Slots, Playback.Revision))
        {
            AnimInstance.Montage_Stop(0.1f, Playback.Montage);
        }
    }

    AnimationData.ActiveMontages.RemoveAll([&AnimInstance, &AnimationData](const FBBBCharacterMontagePlaybackEntry &Playback)
    {
        return !Playback.Montage
            || !AnimInstance.Montage_IsPlaying(Playback.Montage)
            || !ContainsRevision(AnimationData.Slots, Playback.Revision);
    });

    ForEachSlot(AnimationData.Slots, [&AnimInstance, &AnimationData](const FBBBCharacterMontageSlot &Slot)
    {
        if (!Slot.Desired.Montage || Slot.Revision == 0)
        {
            return;
        }
        const bool bPlaying = AnimationData.ActiveMontages.ContainsByPredicate(
            [&Slot](const FBBBCharacterMontagePlaybackEntry &Playback)
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

        FBBBCharacterMontagePlaybackEntry &Playback = AnimationData.ActiveMontages.AddDefaulted_GetRef();
        Playback.Montage = Slot.Desired.Montage;
        Playback.Revision = Slot.Revision;
    });
}
