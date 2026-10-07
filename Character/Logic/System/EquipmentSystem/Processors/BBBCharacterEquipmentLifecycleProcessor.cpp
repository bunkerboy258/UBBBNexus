#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentLifecycleProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentUnequipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/AuthorityFact/Equipment/FBBBEquipmentUnequipAuthorityFactPacket.h"
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
    if (Selection.ActiveMainHandInstance == Context.DesiredEquipment
        && (!Context.bIsMirror || Context.DesiredGeneration == 0 || Selection.ActiveGeneration == Context.DesiredGeneration))
    {
        return;
    }

    ABBBEquipment *Previous = Selection.ActiveMainHandInstance;
    if (IsValid(Previous))
    {
        if (Context.bIsMirror)
        {
            Previous->SubmitInput(FBBBEquipmentUnequipAuthorityFactPacket{});
        }
        if (!Context.bIsMirror)
        {
            Previous->SubmitInput(FBBBEquipmentUnequipLocalControlPacket{});
        }
        Detach(&Context.CharacterMesh, *Previous);
        if (Context.bIsMirror)
        {
            Destroy(&Context.CharacterMesh, *Previous);
        }
    }
    Selection.ActiveMainHandInstance = nullptr;
    Selection.ActiveEquipmentId = NAME_None;
    if (Context.bIsMirror)
    {
        Selection.ActiveGeneration = Context.DesiredGeneration;
    }
    if (!Context.bIsMirror)
    {
        ++Selection.ActiveGeneration;
    }
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
}

void FBBBCharacterEquipmentLifecycleProcessor::Shutdown(FBBBCharacterEquipmentUpdateContext &Context)
{
    auto &Selection = Context.RuntimeData.Equipment.EquipmentSelectionState;
    ABBBEquipment *Active = Selection.ActiveMainHandInstance.Get();
    if (IsValid(Active))
    {
        if (Context.bIsMirror)
        {
            Active->SubmitInput(FBBBEquipmentUnequipAuthorityFactPacket{});
        }
        if (!Context.bIsMirror)
        {
            Active->SubmitInput(FBBBEquipmentUnequipLocalControlPacket{});
        }
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
    FBBBCharacterUpdatePipeline::RegisterEquipmentTicks(CharacterMesh, Equipment);
    Equipment.SetActorTickEnabled(true);
    return true;
}

void FBBBCharacterEquipmentLifecycleProcessor::Detach(
    USkeletalMeshComponent *CharacterMesh, ABBBEquipment &Equipment)
{
    if (CharacterMesh)
    {
        FBBBCharacterUpdatePipeline::UnregisterEquipmentTicks(*CharacterMesh, Equipment);
    }

    Equipment.SetActorHiddenInGame(true);
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
