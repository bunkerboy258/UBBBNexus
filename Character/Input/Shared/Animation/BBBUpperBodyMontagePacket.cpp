#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBUpperBodyMontagePacket.h"

#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "Animation/AnimMontage.h"

bool FBBBUpperBodyMontagePacket::CanApply(const FBBBCharacterInputContext &) const
{
    if (!Montage)
    {
        return true;
    }

    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        if (Track.SlotName == BBBCharacterMontageSlots::UpperBody)
        {
            return true;
        }
    }

    ensureMsgf(false, TEXT("蒙太奇不包含目标槽位 UpperBody"));
    return false;
}

void FBBBUpperBodyMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.AnimationMontageState.UpperBodyMontageRequest = Montage.Get();
    Context.AnimationMontageState.bUpperBodyMontageRequestPending = true;
}
