#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/AnimationSystem/Processors/BBBSMGPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/AnimationSystem/DomainData/Context/BBBSMGAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/RuntimeData/BBBSMGRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/BBBSMGEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Config/BBBSMGDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyAdditiveMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBAdditiveHitReactMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

void FBBBSMGPresentationProcessor::SubmitCharacterMontage(
    const FBBBSMGAnimationUpdateContext &Context, UAnimMontage *Montage)
{
    if (!Montage || !Context.Equipment.IsEquipped())
    {
        return;
    }

    const bool bMirror = !Context.bCausal;

    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        const EBBBCharacterMontageSlot Slot = ABBBCharacter::ClassifyMontageSlot(Track.SlotName);
        if (Slot == EBBBCharacterMontageSlot::FullBody)
        {
            if (bMirror)
            {
                Context.Character.SubmitInput(FBBBFullBodyMontageAuthorityFactPacket{Montage});
                continue;
            }

            Context.Character.SubmitInput(FBBBFullBodyMontageLocalControlPacket{Montage});
            continue;
        }

        if (Slot == EBBBCharacterMontageSlot::UpperBody)
        {
            if (bMirror)
            {
                Context.Character.SubmitInput(FBBBUpperBodyMontageAuthorityFactPacket{Montage});
                continue;
            }

            Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{Montage});
            continue;
        }

        if (Slot == EBBBCharacterMontageSlot::FullBodyAdditivePreAim)
        {
            if (bMirror)
            {
                Context.Character.SubmitInput(FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket{Montage});
                continue;
            }

            Context.Character.SubmitInput(FBBBFullBodyAdditivePreAimMontageLocalControlPacket{Montage});
            continue;
        }

        if (Slot == EBBBCharacterMontageSlot::UpperBodyAdditive)
        {
            if (bMirror)
            {
                Context.Character.SubmitInput(FBBBUpperBodyAdditiveMontageAuthorityFactPacket{Montage});
                continue;
            }

            Context.Character.SubmitInput(FBBBUpperBodyAdditiveMontageLocalControlPacket{Montage});
            continue;
        }

        if (Slot == EBBBCharacterMontageSlot::AdditiveHitReact)
        {
            if (bMirror)
            {
                Context.Character.SubmitInput(FBBBAdditiveHitReactMontageAuthorityFactPacket{Montage});
                continue;
            }

            Context.Character.SubmitInput(FBBBAdditiveHitReactMontageLocalControlPacket{Montage});
            continue;
        }

        ensureMsgf(false, TEXT("装备蒙太奇使用未注册角色槽位 %s"), *Track.SlotName.ToString());
    }
}

void FBBBSMGPresentationProcessor::PlayEquipmentMontage(
    const FBBBSMGAnimationUpdateContext &Context, UAnimMontage *Montage)
{
    UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance();
    if (ensureMsgf(Animation && Montage, TEXT("装备表现缺少动画实例或蒙太奇")))
    {
        Animation->PlayEquipmentMontage(*Montage);
    }
}

void FBBBSMGPresentationProcessor::PlayFire(const FBBBSMGAnimationUpdateContext &Context)
{
    PlayEquipmentMontage(Context, Context.Definition.EquipmentFireMontage);

}
