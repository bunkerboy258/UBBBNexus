#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Definition/BBBEquipmentDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ABBBEquipment *FBBBCharacterEquipmentLifecycleProcessor::Create(
    ABBBCharacter &Character, TSubclassOf<ABBBEquipment> EquipmentClass)
{
    // 创建装备前确认世界和角色网格有效
    UWorld *World = Character.GetWorld();
    USkeletalMeshComponent *CharacterMesh = Character.GetMesh();
    if (!World || !CharacterMesh)
    {
        return nullptr;
    }

    if (!ensureMsgf(
        EquipmentClass,
        TEXT("角色 %s 无法创建空装备类"),
        *Character.GetName()))
    {
        return nullptr;
    }

    ABBBEquipment *Equipment = World->SpawnActorDeferred<ABBBEquipment>(
        EquipmentClass, FTransform::Identity, &Character, &Character,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!Equipment)
    {
        return nullptr;
    }

    Equipment->SetActorHiddenInGame(true);
    UGameplayStatics::FinishSpawningActor(Equipment, FTransform::Identity);
    if (!Equipment->InitializeEquipment())
    {
        Equipment->Destroy();
        return nullptr;
    }

    // 建立装备演员与角色和网格的更新依赖
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
    if (!(CharacterAnim && WeaponAnim && Equipment.GetDefinition()
        && Equipment.GetOwner() == CharacterMesh.GetOwner()
        && !AttachmentSocketName.IsNone() && CharacterMesh.DoesSocketExist(AttachmentSocketName)))
    {
        return false;
    }

    if (!Equipment.AttachToComponent(
        &CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        AttachmentSocketName))
    {
        return false;
    }

    const UBBBEquipmentDefinition *Definition = Equipment.GetDefinition();
    if (!Definition)
    {
        return false;
    }

    Equipment.SetActorRelativeTransform(Definition->SpawnOffset);
    Equipment.SetActorHiddenInGame(false);
    CharacterAnim->BindWeaponAnimInstance(WeaponAnim);
    // 让武器网格等待角色网格完成更新
    WeaponMesh->PrimaryComponentTick.AddPrerequisite(&CharacterMesh, CharacterMesh.PrimaryComponentTick);
    return true;
}

void FBBBCharacterEquipmentLifecycleProcessor::Detach(
    USkeletalMeshComponent *CharacterMesh, ABBBEquipment &Equipment)
{
    // 分离前移除武器网格对角色网格的更新依赖
    USkeletalMeshComponent *WeaponMesh = Equipment.GetEquipmentSkeletalMesh();
    if (CharacterMesh && WeaponMesh)
    {
        WeaponMesh->PrimaryComponentTick.RemovePrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);
        UBBBAnimInstance *CharacterAnim = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
        UBBBEquipmentAnimInstance *WeaponAnim = Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance());
        if (CharacterAnim && CharacterAnim->TryGetWeaponAnimInstance() == WeaponAnim)
        {
            // 只有当前绑定的武器动画实例才允许被清空
            CharacterAnim->BindWeaponAnimInstance(nullptr);
        }
    }

    Equipment.SetActorHiddenInGame(true);
    Equipment.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void FBBBCharacterEquipmentLifecycleProcessor::Destroy(
    USkeletalMeshComponent *CharacterMesh, ABBBEquipment &Equipment)
{
    // 销毁前先执行统一分离流程
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
    // 移除全部更新依赖后销毁装备演员
    Equipment.Destroy();
}
