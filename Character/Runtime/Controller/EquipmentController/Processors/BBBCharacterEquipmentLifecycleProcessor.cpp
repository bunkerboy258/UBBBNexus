#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentLifecycleProcessor.h"

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
    // 创建装备前确认世界和角色网格有效
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

    // 先写入实例定义和镜像状态再完成演员生成
    Equipment->Definition = &Definition;
    Equipment->InstanceId = FGuid::NewGuid();
    Equipment->bIsMirror = bIsMirror;
    Equipment->SetActorHiddenInGame(true);
    UGameplayStatics::FinishSpawningActor(Equipment, FTransform::Identity);
    if (!FBBBEquipmentInitializer::Initialize(*Equipment, *CharacterMesh, Character))
    {
        // 初始化失败时立即销毁未完成的装备演员
        UE_LOG(LogTemp, Error, TEXT("[UBBBC]Equipment initialization failed Definition=%s"), *Definition.GetPathName());
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
    // 附着前确认动画实例装备定义和插槽全部有效
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

    // 应用装备生成偏移并绑定武器动画实例
    Equipment.SetActorRelativeTransform(Equipment.Definition->EquipFragment.Get().GetSpawnOffset());
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

    // 取消装备待处理动作并隐藏后保持世界位置分离
    Equipment.GetExternalAPI().SubmitCancelPendingActions();
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
