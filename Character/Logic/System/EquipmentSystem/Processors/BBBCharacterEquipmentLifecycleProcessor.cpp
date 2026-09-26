#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void FBBBCharacterEquipmentLifecycleProcessor::Update(FBBBCharacterEquipmentUpdateContext &Context) const
{
    if (!Context.bHasSelectionResult)
    {
        return;
    }

    auto &Selection = Context.SelectionState;
    auto &Inventory = Context.InventoryState;
    if (Context.PendingEquipmentClass)
    {
        ABBBEquipment *Created = Create(Context.Character, Context.PendingEquipmentClass);
        if (!Created)
        {
            return;
        }

        Selection.DesiredMainHandInstance = Created;
    }

    // 已销毁对象不能继续被视为有效持有关系
    if (!IsValid(Selection.DesiredMainHandInstance))
    {
        Selection.DesiredMainHandInstance = nullptr;
    }

    if (Selection.ActiveMainHandInstance == Selection.DesiredMainHandInstance
        && IsValid(Selection.ActiveMainHandInstance))
    {
        return;
    }

    ABBBEquipment *Previous = Selection.ActiveMainHandInstance;
    if (IsValid(Previous))
    {
        Previous->OnUnequipped();
        if (Context.bIsMirror || Context.bCreateRequested)
        {
            Destroy(&Context.CharacterMesh, *Previous);
        }
        if (!Context.bIsMirror && !Context.bCreateRequested)
        {
            Detach(&Context.CharacterMesh, *Previous);
        }
    }

    for (FBBBCharacterItem &Item : Inventory.BackpackSlots)
    {
        if (!IsValid(Item.ItemActor.Get()))
        {
            Item.ItemActor = nullptr;
        }
    }

    for (FBBBCharacterItem &Item : Inventory.ItemBarSlots)
    {
        if (!IsValid(Item.ItemActor.Get()))
        {
            Item.ItemActor = nullptr;
        }
    }

    ABBBEquipment *Desired = Selection.DesiredMainHandInstance;
    Selection.ActiveMainHandInstance = nullptr;
    Selection.ActiveEquipmentId = NAME_None;
    if (!Desired)
    {
        return;
    }

    if (!ensureMsgf(Attach(Context.CharacterMesh, Context.RightHandWeaponSocketName, *Desired),
        TEXT("装备挂接失败 %s"), *GetNameSafe(Desired)))
    {
        Destroy(&Context.CharacterMesh, *Desired);
        Selection.DesiredMainHandInstance = nullptr;
        return;
    }

    Selection.ActiveMainHandInstance = Desired;
    Selection.ActiveEquipmentId = Desired->GetEquipmentId();
    if (Context.bCreateRequested && Context.PendingEquipmentClass)
    {
        for (FBBBCharacterItem &Item : Inventory.BackpackSlots)
        {
            if (!Item.ItemActor)
            {
                Item.ItemActor = Desired;
                break;
            }
        }

        for (FBBBCharacterItem &Item : Inventory.ItemBarSlots)
        {
            if (!Item.ItemActor)
            {
                Item.ItemActor = Desired;
                break;
            }
        }
    }

    if (Desired->IsMirror())
    {
        Desired->SubmitInput(FBBBEquipmentEquipAuthorityFactPacket{});
        return;
    }

    Desired->SubmitInput(FBBBEquipmentEquipLocalControlPacket{});
}

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
    if (!Equipment->IsInitialized())
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
    UBBBEquipmentAnimInstance *WeaponAnim = WeaponMesh
        ? Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance())
        : nullptr;
    FTransform AttachmentOffset;
    if (!(WeaponAnim && Equipment.TryGetAttachmentOffset(AttachmentOffset)
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

    Equipment.SetActorRelativeTransform(AttachmentOffset);
    Equipment.SetActorHiddenInGame(false);
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
