#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBFullBodyMontagePacket.h"

#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "Animation/AnimMontage.h"

bool FBBBFullBodyMontagePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    if (!Context.AnimationInstance)
    {
        return false;
    }

    if (!Montage)
    {
        return true;
    }

    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        if (Track.SlotName == BBBCharacterMontageSlots::FullBody)
        {
            return true;
        }
    }

    ensureMsgf(false, TEXT("蒙太奇不包含目标槽位 FullBody"));
    return false;
}

void FBBBFullBodyMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.AnimationInstance->RegisterMontageContribution(BBBCharacterMontageSlots::FullBody, Montage.Get());
}
