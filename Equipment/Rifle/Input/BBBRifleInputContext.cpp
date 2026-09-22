#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UWorld *FBBBRifleInputContext::GetWorld() const
{
    return WeaponMesh.GetWorld();
}

void FBBBRifleInputContext::SubmitCharacterMontage(
    UAnimMontage *Montage,
    const int32 Sequence,
    const bool bReload) const
{
    if (!Montage)
    {
        return;
    }

    BBBCharacterMontageInput::Submit(Character, *Montage, Sequence, bReload);
}

void FBBBRifleInputContext::PlayEquipmentMontage(UAnimMontage *Montage) const
{
    if (!Montage)
    {
        ensureMsgf(false, TEXT("步枪缺少需要播放的装备蒙太奇"));
        return;
    }

    UBBBEquipmentAnimInstance *AnimationInstance = Equipment.GetEquipmentAnimationInstance();
    if (!ensureMsgf(AnimationInstance, TEXT("步枪缺少装备动画实例")))
    {
        return;
    }

    AnimationInstance->PlayEquipmentMontage(*Montage);
}

void FBBBRifleInputContext::PlayFireSound() const
{
    UWorld *World = GetWorld();
    if (Definition.FireSound && World
        && WeaponMesh.DoesSocketExist(Definition.MuzzleSocketName))
    {
        const FVector Location = WeaponMesh.GetSocketLocation(Definition.MuzzleSocketName);
        UGameplayStatics::SpawnSoundAtLocation(World, Definition.FireSound, Location);
    }
}
