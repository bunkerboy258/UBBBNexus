#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBFullBodyAdditivePreAimMontagePacket.h"

#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "Animation/AnimMontage.h"

bool FBBBFullBodyAdditivePreAimMontagePacket::CanApply(const FBBBCharacterInputContext &Context) const
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
        if (Track.SlotName == BBBCharacterMontageSlots::FullBodyAdditivePreAim)
        {
            return true;
        }
    }

    ensureMsgf(false, TEXT("蒙太奇不包含目标槽位 FullBodyAdditivePreAim"));
    return false;
}

void FBBBFullBodyAdditivePreAimMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.AnimationInstance->RegisterMontageContribution(BBBCharacterMontageSlots::FullBodyAdditivePreAim, Montage.Get());
}
