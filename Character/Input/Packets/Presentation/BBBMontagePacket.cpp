#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"
#include "Animation/AnimMontage.h"

namespace BBBCharacterMontageSlots
{
    const FName FullBody(TEXT("FullBody"));
    const FName UpperBody(TEXT("UpperBody"));
    const FName FullBodyAdditivePreAim(TEXT("FullBodyAdditivePreAim"));
    const FName UpperBodyAdditive(TEXT("UpperBodyAdditive"));
    const FName AdditiveHitReact(TEXT("AdditiveHitReact"));
}

bool FBBBMontagePacketData::IsValid() const
{
    return Montage != nullptr && FMath::IsFinite(PlayRate) && PlayRate > 0.0f;
}

bool FBBBMontagePacketData::CanApplyToSlot(
    const FBBBCharacterPacketContext &Context,
    const FName Slot) const
{
    FBBBCharacterMontagePacket Request;
    Request.Montage = Montage;
    Request.PlayRate = PlayRate;
    Request.Sequence = Sequence;
    Request.bReload = bReload;
    return Request.CanApply(Context.Animation, Context.Operation, Slot);
}

void FBBBMontagePacketData::ApplyToSlot(
    FBBBCharacterPacketContext &Context,
    const FName Slot) const
{
    FBBBCharacterMontagePacket Request;
    Request.Montage = Montage;
    Request.PlayRate = PlayRate;
    Request.Sequence = Sequence;
    Request.bReload = bReload;
    Request.Apply(Context.Animation, Slot);
}

//------------------------------------------------------------------------------

bool FBBBFullBodyMontagePacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::FullBody);
}

void FBBBFullBodyMontagePacket::Apply(FBBBCharacterPacketContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::FullBody);
}

bool FBBBUpperBodyMontagePacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::UpperBody);
}

void FBBBUpperBodyMontagePacket::Apply(FBBBCharacterPacketContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::UpperBody);
}

bool FBBBFullBodyAdditivePreAimMontagePacket::CanApply(
    const FBBBCharacterPacketContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::FullBodyAdditivePreAim);
}

void FBBBFullBodyAdditivePreAimMontagePacket::Apply(FBBBCharacterPacketContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::FullBodyAdditivePreAim);
}

bool FBBBUpperBodyAdditiveMontagePacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::UpperBodyAdditive);
}

void FBBBUpperBodyAdditiveMontagePacket::Apply(FBBBCharacterPacketContext &Context) const
{
    ApplyToSlot(Context, BBBCharacterMontageSlots::UpperBodyAdditive);
}

bool FBBBAdditiveHitReactMontagePacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    return CanApplyToSlot(Context, BBBCharacterMontageSlots::AdditiveHitReact);
}

void FBBBAdditiveHitReactMontagePacket::Apply(FBBBCharacterPacketContext &Context) const
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
    if (!ensureMsgf(!Montage.SlotAnimTracks.IsEmpty(),
        TEXT("[UBBBC]Montage input requires at least one configured slot Asset=%s"),
        *Montage.GetPathName()))
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

        ensureMsgf(false, TEXT("[UBBBC]Montage uses unsupported slot Asset=%s Slot=%s"),
            *Montage.GetPathName(), *Track.SlotName.ToString());
        bSubmittedAll = false;
    }

    return bSubmittedAll;
}
