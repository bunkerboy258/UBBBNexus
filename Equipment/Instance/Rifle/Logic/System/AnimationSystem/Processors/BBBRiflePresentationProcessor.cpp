#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRiflePresentationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Input/Local/Camera/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"

void FBBBRiflePresentationProcessor::SubmitCharacterMontage(
    const FBBBRifleUpdateContext &Context, UAnimMontage *Montage, const bool bClear)
{
    if (!Montage || !Context.Equipment.IsEquipped())
    {
        return;
    }

    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        if (Track.SlotName == BBBCharacterMontageSlots::FullBody)
        {
            Context.Character.SubmitInput(FBBBFullBodyMontagePacket{bClear ? nullptr : Montage});
            continue;
        }

        if (Track.SlotName == BBBCharacterMontageSlots::UpperBody)
        {
            Context.Character.SubmitInput(FBBBUpperBodyMontagePacket{bClear ? nullptr : Montage});
            continue;
        }

        if (Track.SlotName == BBBCharacterMontageSlots::FullBodyAdditivePreAim)
        {
            Context.Character.SubmitInput(FBBBFullBodyAdditivePreAimMontagePacket{bClear ? nullptr : Montage});
            continue;
        }

        if (Track.SlotName == BBBCharacterMontageSlots::UpperBodyAdditive)
        {
            Context.Character.SubmitInput(FBBBUpperBodyAdditiveMontagePacket{bClear ? nullptr : Montage});
            continue;
        }

        if (Track.SlotName == BBBCharacterMontageSlots::AdditiveHitReact)
        {
            Context.Character.SubmitInput(FBBBAdditiveHitReactMontagePacket{bClear ? nullptr : Montage});
            continue;
        }

        ensureMsgf(false, TEXT("装备蒙太奇使用未注册角色槽位 %s"), *Track.SlotName.ToString());
    }
}

void FBBBRiflePresentationProcessor::PlayEquipmentMontage(
    const FBBBRifleUpdateContext &Context, UAnimMontage *Montage)
{
    UBBBEquipmentAnimInstance *Animation = Context.Equipment.GetEquipmentAnimationInstance();
    if (ensureMsgf(Animation && Montage, TEXT("装备表现缺少动画实例或蒙太奇")))
    {
        Animation->PlayEquipmentMontage(*Montage);
    }
}

void FBBBRiflePresentationProcessor::PlayFire(const FBBBRifleUpdateContext &Context)
{
    if (Context.Definition.FireSound)
    {
        UGameplayStatics::SpawnSoundAtLocation(&Context.World, Context.Definition.FireSound,
            Context.WeaponMesh.GetSocketLocation(Context.Definition.MuzzleSocketName));
    }

    PlayEquipmentMontage(Context, Context.Definition.EquipmentFireMontage);
    if (Context.Equipment.IsMirror())
    {
        return;
    }

    FBBBCameraPacket Packet;
    Packet.Impulse = FVector2D(
        Context.Definition.VerticalRecoilAmount + FMath::FRandRange(
            -Context.Definition.VerticalRecoilRandom, Context.Definition.VerticalRecoilRandom),
        Context.Definition.HorizontalRecoilAmount + FMath::FRandRange(
            -Context.Definition.HorizontalRecoilRandom, Context.Definition.HorizontalRecoilRandom));
    Packet.RecoverySpeed = Context.Definition.RecoilRecoverySpeed;
    Context.Character.SubmitInput(Packet);
}
