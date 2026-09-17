#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Initialization/BBBEquipmentInitializer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ABBBEquipment *FBBBCharacterEquipmentLifecycleProcessor::Create(
    ABBBCharacter &Character, UBBBEquipmentDefinition &Definition, const bool bIsMirror)
{
    UWorld *World = Character.GetWorld();
    USkeletalMeshComponent *CharacterMesh = Character.GetMesh();
    if (!ensureMsgf(World && CharacterMesh, TEXT("[UBBBC]Equipment creation dependencies are invalid")))
    {
        return nullptr;
    }

    ABBBEquipment *Equipment = World->SpawnActorDeferred<ABBBEquipment>(
        ABBBEquipment::StaticClass(), FTransform::Identity, &Character, &Character,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!ensureMsgf(Equipment, TEXT("[UBBBC]Equipment actor creation failed")))
    {
        return nullptr;
    }

    Equipment->Definition = &Definition;
    Equipment->InstanceId = FGuid::NewGuid();
    Equipment->bIsMirror = bIsMirror;
    Equipment->SetActorHiddenInGame(true);
    UGameplayStatics::FinishSpawningActor(Equipment, FTransform::Identity);
    if (!FBBBEquipmentInitializer::Initialize(*Equipment, *CharacterMesh, Character.GetExternalAPI()))
    {
        UE_LOG(LogTemp, Error, TEXT("[UBBBC]Equipment initialization failed Definition=%s"), *Definition.GetPathName());
        Equipment->Destroy();
        return nullptr;
    }

    Equipment->PrimaryActorTick.AddPrerequisite(&Character, Character.PrimaryActorTick);
    Equipment->PrimaryActorTick.AddPrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);
    Equipment->SetActorTickEnabled(true);
    return Equipment;
}

bool FBBBCharacterEquipmentLifecycleProcessor::Attach(
    USkeletalMeshComponent &CharacterMesh, const FName AttachmentSocketName, ABBBEquipment &Equipment)
{
    USkeletalMeshComponent *WeaponMesh = Equipment.GetEquipmentSkeletalMesh();
    UBBBAnimInstance *CharacterAnim = Cast<UBBBAnimInstance>(CharacterMesh.GetAnimInstance());
    UBBBEquipmentAnimInstance *WeaponAnim = WeaponMesh
        ? Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance())
        : nullptr;
    if (!ensureMsgf(CharacterAnim && WeaponAnim && Equipment.Definition
        && Equipment.Definition->EquipFragment.IsValid()
        && Equipment.GetOwner() == CharacterMesh.GetOwner()
        && !AttachmentSocketName.IsNone() && CharacterMesh.DoesSocketExist(AttachmentSocketName),
        TEXT("[UBBBC]Equipment attachment or animation dependencies are invalid")))
    {
        return false;
    }

    if (!ensureMsgf(Equipment.AttachToComponent(&CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachmentSocketName),
        TEXT("[UBBBC]Equipment attachment failed")))
    {
        return false;
    }

    Equipment.SetActorRelativeTransform(Equipment.Definition->EquipFragment.Get().GetSpawnOffset());
    Equipment.SetActorHiddenInGame(false);
    CharacterAnim->BindWeaponAnimInstance(WeaponAnim);
    WeaponMesh->PrimaryComponentTick.AddPrerequisite(&CharacterMesh, CharacterMesh.PrimaryComponentTick);
    return true;
}

void FBBBCharacterEquipmentLifecycleProcessor::Detach(
    USkeletalMeshComponent *CharacterMesh, ABBBEquipment &Equipment)
{
    USkeletalMeshComponent *WeaponMesh = Equipment.GetEquipmentSkeletalMesh();
    if (CharacterMesh && WeaponMesh)
    {
        WeaponMesh->PrimaryComponentTick.RemovePrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);
        UBBBAnimInstance *CharacterAnim = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
        UBBBEquipmentAnimInstance *WeaponAnim = Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance());
        if (CharacterAnim && CharacterAnim->TryGetWeaponAnimInstance() == WeaponAnim)
        {
            CharacterAnim->BindWeaponAnimInstance(nullptr);
        }
    }

    Equipment.GetExternalAPI().SubmitCancelPendingActions();
    Equipment.SetActorHiddenInGame(true);
    Equipment.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void FBBBCharacterEquipmentLifecycleProcessor::Destroy(
    USkeletalMeshComponent *CharacterMesh, ABBBEquipment &Equipment)
{
    Detach(CharacterMesh, Equipment);
    Equipment.SetActorTickEnabled(false);
    if (CharacterMesh)
    {
        Equipment.PrimaryActorTick.RemovePrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);
    }
    if (AActor *Holder = Equipment.GetOwner())
    {
        Equipment.PrimaryActorTick.RemovePrerequisite(Holder, Holder->PrimaryActorTick);
    }
    Equipment.Destroy();
}
