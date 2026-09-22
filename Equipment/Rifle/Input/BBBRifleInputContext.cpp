#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UWorld *FBBBRifleInputContext::GetWorld() const
{
    return WeaponMesh.GetWorld();
}

void FBBBRifleInputContext::SubmitMontage(
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

void FBBBRifleInputContext::PlayFirePresentation(const int32 Sequence) const
{
    UWorld *World = GetWorld();
    if (Definition.FireSound && World
        && WeaponMesh.DoesSocketExist(Definition.MuzzleSocketName))
    {
        const FVector Location = WeaponMesh.GetSocketLocation(Definition.MuzzleSocketName);
        UGameplayStatics::SpawnSoundAtLocation(World, Definition.FireSound, Location);
    }

    SubmitMontage(Definition.FireMontage, Sequence, false);
}
