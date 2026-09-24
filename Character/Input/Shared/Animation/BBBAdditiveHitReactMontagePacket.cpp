#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBAdditiveHitReactMontagePacket.h"

#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "Animation/AnimMontage.h"

bool FBBBAdditiveHitReactMontagePacket::CanApply(const FBBBCharacterInputContext &Context) const
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
        if (Track.SlotName == BBBCharacterMontageSlots::AdditiveHitReact)
        {
            return true;
        }
    }

    ensureMsgf(false, TEXT("蒙太奇不包含目标槽位 AdditiveHitReact"));
    return false;
}

void FBBBAdditiveHitReactMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.AnimationInstance->RegisterMontageContribution(BBBCharacterMontageSlots::AdditiveHitReact, Montage.Get());
}
