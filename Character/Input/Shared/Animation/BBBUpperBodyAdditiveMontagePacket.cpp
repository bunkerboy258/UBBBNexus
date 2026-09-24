#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBUpperBodyAdditiveMontagePacket.h"

#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "Animation/AnimMontage.h"

bool FBBBUpperBodyAdditiveMontagePacket::CanApply(const FBBBCharacterInputContext &) const
{
    if (!Montage)
    {
        return true;
    }

    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        if (Track.SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
        {
            return true;
        }
    }

    ensureMsgf(false, TEXT("蒙太奇不包含目标槽位 UpperBodyAdditive"));
    return false;
}

void FBBBUpperBodyAdditiveMontagePacket::Apply(FBBBCharacterInputContext &Context) const
{
    Context.AnimationMontageState.UpperBodyAdditiveMontageRequest = Montage.Get();
    Context.AnimationMontageState.bUpperBodyAdditiveMontageRequestPending = true;
}
