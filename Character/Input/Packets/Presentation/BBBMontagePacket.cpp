#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"

#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Template/BBBEquipment.h"
#include "Animation/AnimMontage.h"

bool FBBBMontagePacketData::IsValid() const
{
    return Montage != nullptr && SourceEquipment.IsValid();
}

bool FBBBMontagePacketData::CanApplyToSlot(
    const FBBBCharacterInputContext &Context,
    const FName Slot) const
{
    if (Context.SelectedEquipment || !Montage
        || SourceEquipment.Get() != Context.Equipment.ActiveMainHandInstance)
    {
        return false;
    }

    if (!Context.AnimationInstance || !Context.AnimationInstance->FindMontageContribution(Slot))
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
    if (!Context.AnimationInstance)
    {
        return;
    }

    Context.AnimationInstance->RegisterMontageContribution(Slot, *Montage);
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
    ABBBEquipment &SourceEquipment)
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
        Data.SourceEquipment = &SourceEquipment;

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
