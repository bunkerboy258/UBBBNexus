#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBCharacterMontageRequestState.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/State/BBBCharacterParseState.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterMontageRequestState::BeginFrame(
    FBBBAnimationState &Animation,
    const FBBBCharacterParseState &Operation)
{
    Animation.Slots.ForEach([&Operation](FBBBCharacterMontageSlotState &Slot)
    {
        if (Operation.IsEquipmentSwitchPending()
            || (Operation.IsCancelledReloadSequence(Slot.Desired.Sequence) && Slot.Desired.bReload))
        {
            Slot.Desired = FBBBCharacterMontageRequestState();
            Slot.Revision = 0;
        }
    });
}

//------------------------------------------------------------------------------

bool FBBBCharacterMontageRequestState::CanApply(
    const FBBBAnimationState &Animation,
    const FBBBCharacterParseState &Operation,
    const FName Slot) const
{
    if (Operation.IsEquipmentSwitchPending() || !Montage)
    {
        return false;
    }

    if (bReload
        && (!Operation.IsCurrentReloadSequence(Sequence)
            || Operation.IsCancelledReloadSequence(Sequence)))
    {
        return false;
    }

    const FBBBCharacterMontageSlotState *TargetSlot = Animation.Slots.Find(Slot);
    if (!TargetSlot)
    {
        return false;
    }

    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        if (Track.SlotName == Slot)
        {
            return true;
        }
    }

    return false;
}

void FBBBCharacterMontageRequestState::Apply(
    FBBBAnimationState &Animation,
    const FName SlotName) const
{
    FBBBCharacterMontageSlotState *TargetSlot = Animation.Slots.Find(SlotName);
    if (!TargetSlot)
    {
        return;
    }

    uint64 SharedRevision = 0;

    Animation.Slots.ForEach([this, &SharedRevision](const FBBBCharacterMontageSlotState &Slot)
    {
        if (Slot.Desired.Montage == Montage
            && Slot.Desired.Sequence == Sequence
            && Slot.Revision != 0)
        {
            SharedRevision = Slot.Revision;
        }
    });

    if (SharedRevision != 0)
    {
        TargetSlot->Desired = *this;
        TargetSlot->Revision = SharedRevision;
        return;
    }

    Animation.Slots.ForEach([this](FBBBCharacterMontageSlotState &Slot)
    {
        if (Slot.Desired.Montage && Slot.Desired.Montage->GetGroupName() == Montage->GetGroupName())
        {
            Slot.Desired = FBBBCharacterMontageRequestState();
            Slot.Revision = 0;
        }
    });

    TargetSlot->Desired = *this;
    TargetSlot->Revision = Animation.NextRevision++;
}
