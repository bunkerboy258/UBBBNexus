#include "BBBWork/UBBBNexus/Equipment/Rifle/Processors/BBBRiflePresentationProcessor.h"

#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/Context/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void FBBBRiflePresentationProcessor::SubmitCharacterMontage(
    const FBBBRifleInputContext &Context,
    UAnimMontage *Montage,
    const int32 Sequence,
    const bool bReload)
{
    if (!Montage)
    {
        return;
    }

    BBBCharacterMontageInput::Submit(Context.Character, *Montage, Sequence, bReload);
}

void FBBBRiflePresentationProcessor::PlayEquipmentMontage(
    const FBBBRifleInputContext &Context,
    UAnimMontage *Montage)
{
    if (!Montage)
    {
        ensureMsgf(false, TEXT("步枪缺少需要播放的装备蒙太奇"));
        return;
    }

    UBBBEquipmentAnimInstance *AnimationInstance = Context.Equipment.GetEquipmentAnimationInstance();
    if (!ensureMsgf(AnimationInstance, TEXT("步枪缺少装备动画实例")))
    {
        return;
    }

    AnimationInstance->PlayEquipmentMontage(*Montage);
}

void FBBBRiflePresentationProcessor::PlayFireSound(const FBBBRifleInputContext &Context)
{
    UWorld *World = Context.World;
    if (Context.Definition.FireSound && World
        && Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName))
    {
        const FVector Location = Context.WeaponMesh.GetSocketLocation(Context.Definition.MuzzleSocketName);
        UGameplayStatics::SpawnSoundAtLocation(World, Context.Definition.FireSound, Location);
    }
}
