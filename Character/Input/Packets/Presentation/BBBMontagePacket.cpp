#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterOperation.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterMontageRequest.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "Animation/AnimMontage.h"

namespace BBBCharacterMontageSlots
{
    const FName FullBody(TEXT("FullBody"));
    const FName UpperBody(TEXT("UpperBody"));
    const FName FullBodyAdditivePreAim(TEXT("FullBodyAdditivePreAim"));
    const FName UpperBodyAdditive(TEXT("UpperBodyAdditive"));
    const FName AdditiveHitReact(TEXT("AdditiveHitReact"));
}

namespace
{
    FBBBCharacterMontageSlot *FindSlot(
        FBBBCharacterAnimationState &Animation,
        const FName SlotName)
    {
        if (SlotName == BBBCharacterMontageSlots::FullBody)
        {
            return &Animation.Slots.FullBody;
        }

        if (SlotName == BBBCharacterMontageSlots::UpperBody)
        {
            return &Animation.Slots.UpperBody;
        }

        if (SlotName == BBBCharacterMontageSlots::FullBodyAdditivePreAim)
        {
            return &Animation.Slots.FullBodyAdditivePreAim;
        }

        if (SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
        {
            return &Animation.Slots.UpperBodyAdditive;
        }

        if (SlotName == BBBCharacterMontageSlots::AdditiveHitReact)
        {
            return &Animation.Slots.AdditiveHitReact;
        }

        return nullptr;
    }

    const FBBBCharacterMontageSlot *FindSlot(
        const FBBBCharacterAnimationState &Animation,
        const FName SlotName)
    {
        if (SlotName == BBBCharacterMontageSlots::FullBody)
        {
            return &Animation.Slots.FullBody;
        }

        if (SlotName == BBBCharacterMontageSlots::UpperBody)
        {
            return &Animation.Slots.UpperBody;
        }

        if (SlotName == BBBCharacterMontageSlots::FullBodyAdditivePreAim)
        {
            return &Animation.Slots.FullBodyAdditivePreAim;
        }

        if (SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
        {
            return &Animation.Slots.UpperBodyAdditive;
        }

        if (SlotName == BBBCharacterMontageSlots::AdditiveHitReact)
        {
            return &Animation.Slots.AdditiveHitReact;
        }

        return nullptr;
    }

    template<typename TFunction>
    void ForEachSlot(FBBBCharacterMontageSlots &Slots, TFunction &&Function)
    {
        Function(Slots.FullBody);
        Function(Slots.UpperBody);
        Function(Slots.FullBodyAdditivePreAim);
        Function(Slots.UpperBodyAdditive);
        Function(Slots.AdditiveHitReact);
    }
}

bool FBBBMontagePacketData::IsValid() const
{
    return Montage != nullptr && FMath::IsFinite(PlayRate) && PlayRate > 0.0f;
}

bool FBBBMontagePacketData::CanApplyToSlot(
    const FBBBCharacterInputContext &Context,
    const FName Slot) const
{
    if (BBBCharacterOperation::IsEquipmentSwitchPending(Context.Operation) || !Montage)
    {
        return false;
    }

    if (bReload
        && (!BBBCharacterOperation::IsCurrentReloadSequence(Context.Operation, Sequence)
            || BBBCharacterOperation::IsCancelledReloadSequence(Context.Operation, Sequence)))
    {
        return false;
    }

    if (!FindSlot(Context.Animation, Slot))
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

void FBBBMontagePacketData::ApplyToSlot(
    FBBBCharacterInputContext &Context,
    const FName Slot) const
{
    FBBBCharacterMontageSlot *TargetSlot = FindSlot(Context.Animation, Slot);
    if (!TargetSlot)
    {
        return;
    }

    FBBBCharacterMontageRequest Request;
    Request.Montage = Montage;
    Request.PlayRate = PlayRate;
    Request.Sequence = Sequence;
    Request.bReload = bReload;
    uint64 SharedRevision = 0;

    ForEachSlot(Context.Animation.Slots, [&Request, &SharedRevision](const FBBBCharacterMontageSlot &ExistingSlot)
    {
        if (ExistingSlot.Desired.Montage == Request.Montage
            && ExistingSlot.Desired.Sequence == Request.Sequence
            && ExistingSlot.Revision != 0)
        {
            SharedRevision = ExistingSlot.Revision;
        }
    });

    if (SharedRevision != 0)
    {
        TargetSlot->Desired = Request;
        TargetSlot->Revision = SharedRevision;
        return;
    }

    ForEachSlot(Context.Animation.Slots, [&Request](FBBBCharacterMontageSlot &ExistingSlot)
    {
        if (ExistingSlot.Desired.Montage
            && ExistingSlot.Desired.Montage->GetGroupName() == Request.Montage->GetGroupName())
        {
            ExistingSlot.Desired = FBBBCharacterMontageRequest();
            ExistingSlot.Revision = 0;
        }
    });

    TargetSlot->Desired = Request;
    TargetSlot->Revision = Context.Animation.NextRevision++;
}

//------------------------------------------------------------------------------

bool FBBBFullBodyMontagePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::FullBody);
}

void FBBBFullBodyMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::FullBody);
}

bool FBBBUpperBodyMontagePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::UpperBody);
}

void FBBBUpperBodyMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::UpperBody);
}

bool FBBBFullBodyAdditivePreAimMontagePacket::CanApply(
    const FBBBCharacterInputContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::FullBodyAdditivePreAim);
}

void FBBBFullBodyAdditivePreAimMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::FullBodyAdditivePreAim);
}

bool FBBBUpperBodyAdditiveMontagePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::UpperBodyAdditive);
}

void FBBBUpperBodyAdditiveMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::UpperBodyAdditive);
}

bool FBBBAdditiveHitReactMontagePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::AdditiveHitReact);
}

void FBBBAdditiveHitReactMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::AdditiveHitReact);
}

//------------------------------------------------------------------------------

bool BBBCharacterMontageInput::Submit(
    ABBBCharacter &Character,
    UAnimMontage &Montage,
    const float PlayRate,
    const int32 Sequence,
    const bool bReload)
{
    if (Montage.SlotAnimTracks.IsEmpty())
    {
        return false;
    }

    bool bSubmittedAll = true;

    for (const FSlotAnimationTrack &Track : Montage.SlotAnimTracks)
    {
        FBBBMontagePacketData Data;
        Data.Montage = &Montage;
        Data.PlayRate = PlayRate;
        Data.Sequence = Sequence;
        Data.bReload = bReload;

        if (Track.SlotName == BBBCharacterMontageSlots::FullBody)
        {
            FBBBFullBodyMontagePacket Packet;
            static_cast<FBBBMontagePacketData &>(Packet) = Data;
            bSubmittedAll &= Character.SubmitInput(MoveTemp(Packet));
            continue;
        }

        if (Track.SlotName == BBBCharacterMontageSlots::UpperBody)
        {
            FBBBUpperBodyMontagePacket Packet;
            static_cast<FBBBMontagePacketData &>(Packet) = Data;
            bSubmittedAll &= Character.SubmitInput(MoveTemp(Packet));
            continue;
        }

        if (Track.SlotName == BBBCharacterMontageSlots::FullBodyAdditivePreAim)
        {
            FBBBFullBodyAdditivePreAimMontagePacket Packet;
            static_cast<FBBBMontagePacketData &>(Packet) = Data;
            bSubmittedAll &= Character.SubmitInput(MoveTemp(Packet));
            continue;
        }

        if (Track.SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
        {
            FBBBUpperBodyAdditiveMontagePacket Packet;
            static_cast<FBBBMontagePacketData &>(Packet) = Data;
            bSubmittedAll &= Character.SubmitInput(MoveTemp(Packet));
            continue;
        }

        if (Track.SlotName == BBBCharacterMontageSlots::AdditiveHitReact)
        {
            FBBBAdditiveHitReactMontagePacket Packet;
            static_cast<FBBBMontagePacketData &>(Packet) = Data;
            bSubmittedAll &= Character.SubmitInput(MoveTemp(Packet));
            continue;
        }

        bSubmittedAll = false;
    }

    return bSubmittedAll;
}
