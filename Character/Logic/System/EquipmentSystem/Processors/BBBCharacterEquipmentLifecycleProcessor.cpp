#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
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
    auto &Selection = Context.RuntimeData.Equipment.EquipmentSelectionState;
    if (Context.PendingEquipmentClass)
    {
        Context.DesiredEquipment = Create(Context.Character, Context.PendingEquipmentClass);
        if (!Context.DesiredEquipment)
        {
            return;
        }
    }
    if (!IsValid(Context.DesiredEquipment))
    {
        Context.DesiredEquipment = nullptr;
    }
    if (Selection.ActiveMainHandInstance == Context.DesiredEquipment)
    {
        return;
    }

    ABBBEquipment *Previous = Selection.ActiveMainHandInstance;
    if (IsValid(Previous))
    {
        Previous->OnUnequipped();
        Detach(&Context.CharacterMesh, *Previous);
        if (Context.bIsMirror)
        {
            Destroy(&Context.CharacterMesh, *Previous);
        }
    }
    Selection.ActiveMainHandInstance = nullptr;
    Selection.ActiveEquipmentId = NAME_None;
    ABBBEquipment *Desired = Context.DesiredEquipment;
    if (!Desired)
    {
        return;
    }

    if (!ensureMsgf(Attach(Context.CharacterMesh, Context.RightHandWeaponSocketName, *Desired),
        TEXT("装备挂接失败 %s"), *GetNameSafe(Desired)))
    {
        if (Context.bIsMirror)
        {
            Destroy(&Context.CharacterMesh, *Desired);
        }
        return;
    }
    Selection.ActiveMainHandInstance = Desired;
    Selection.ActiveEquipmentId = Desired->GetEquipmentId();
    if (Context.bIsMirror)
    {
        Desired->SubmitInput(FBBBEquipmentEquipAuthorityFactPacket{});
        return;
    }
    Desired->SubmitInput(FBBBEquipmentEquipLocalControlPacket{});
}

void FBBBCharacterEquipmentLifecycleProcessor::Shutdown(FBBBCharacterEquipmentUpdateContext &Context)
{
    auto &Selection = Context.RuntimeData.Equipment.EquipmentSelectionState;
    ABBBEquipment *Active = Selection.ActiveMainHandInstance.Get();
    if (IsValid(Active))
    {
        Active->OnUnequipped();
        Detach(&Context.CharacterMesh, *Active);
        if (Context.bIsMirror)
        {
            Destroy(&Context.CharacterMesh, *Active);
        }
    }
    Selection.ActiveMainHandInstance = nullptr;
    Selection.ActiveEquipmentId = NAME_None;
    Selection.PendingEquipmentId = NAME_None;
    Selection.bHasEquipmentRequest = false;
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

    Equipment->SetActorTickEnabled(false);
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
    Equipment.PrimaryActorTick.AddPrerequisite(CharacterMesh.GetOwner(), CharacterMesh.GetOwner()->PrimaryActorTick);
    Equipment.PrimaryActorTick.AddPrerequisite(&CharacterMesh, CharacterMesh.PrimaryComponentTick);
    Equipment.SetActorTickEnabled(true);
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
    Equipment.SetActorTickEnabled(false);
    if (CharacterMesh)
    {
        Equipment.PrimaryActorTick.RemovePrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);
    }
    if (AActor *Holder = Equipment.GetOwner())
    {
        Equipment.PrimaryActorTick.RemovePrerequisite(Holder, Holder->PrimaryActorTick);
    }
    Equipment.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void FBBBCharacterEquipmentLifecycleProcessor::Destroy(
    USkeletalMeshComponent *CharacterMesh, ABBBEquipment &Equipment)
{
    // 销毁前先执行统一分离流程
    Detach(CharacterMesh, Equipment);
    // 移除全部更新依赖后销毁装备演员
    Equipment.Destroy();
}
