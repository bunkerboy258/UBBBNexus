#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/AnimationSystem/Processors/BBBShotgunPresentationProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/Context/BBBShotgunUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/RuntimeData/BBBShotgunRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBAimImpulseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBAimImpulseAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyAdditiveMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBAdditiveHitReactMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

void FBBBShotgunPresentationProcessor::SubmitCharacterMontage(
    const FBBBShotgunUpdateContext &Context, UAnimMontage *Montage)
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

void FBBBShotgunPresentationProcessor::PlayEquipmentMontage(
    const FBBBShotgunUpdateContext &Context, UAnimMontage *Montage)
{
    UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance();
    if (ensureMsgf(Animation && Montage, TEXT("装备表现缺少动画实例或蒙太奇")))
    {
        Animation->PlayEquipmentMontage(*Montage);
    }
}

void FBBBShotgunPresentationProcessor::PlayFire(const FBBBShotgunUpdateContext &Context)
{
    PlayEquipmentMontage(Context, Context.Definition.EquipmentFireMontage);
    const auto &Hip = Context.Definition.HipFireSettings;
    const auto &Aim = Context.Definition.AimFireSettings;
    const FVector2D Random(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f));
    const FVector2D HipImpulse = Hip.AimImpulseDegrees + Hip.AimImpulseRandomDegrees * Random;
    const FVector2D AimImpulse = Aim.AimImpulseDegrees + Aim.AimImpulseRandomDegrees * Random;
    if (!Context.bCausal)
    {
        Context.Character.SubmitInput(FBBBAimImpulseAuthorityFactPacket{
            HipImpulse, AimImpulse, Context.Definition.AirborneModifiers.AimImpulseScale,
            Hip.AimImpulseRecoverySpeed, Aim.AimImpulseRecoverySpeed,
            Context.Definition.CrouchedModifiers.AimImpulseScale,
            Context.Definition.CrouchedModifiers.AimImpulseRecoveryScale});
        return;
    }

    Context.Character.SubmitInput(FBBBAimImpulseLocalControlPacket{
        HipImpulse, AimImpulse, Context.Definition.AirborneModifiers.AimImpulseScale,
        Hip.AimImpulseRecoverySpeed, Aim.AimImpulseRecoverySpeed,
        Context.Definition.CrouchedModifiers.AimImpulseScale,
        Context.Definition.CrouchedModifiers.AimImpulseRecoveryScale});
}
