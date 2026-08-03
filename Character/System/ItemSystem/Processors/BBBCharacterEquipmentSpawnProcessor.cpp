
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterEquipmentSpawnProcessor.h"
#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"

void FBBBCharacterEquipmentSpawnProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    float WorldTimeSeconds,
    FName RightHandWeaponSocketName,
    UBBBEquipmentCatalog &EquipmentCatalog,
    FBBBCharacterItemRuntimeData &ItemData,
    FBBBCharacterExternalAPI &CharacterAPI) const
{

    FBBBCharacterEquipmentState &Equipment = ItemData.State.Equipment;

    UBBBEquipmentDefinition *Definition = nullptr;
    const FBBBItemInstance *RuntimeItem = nullptr;

    if (Equipment.TargetMode == EBBBEquipmentTargetMode::RuntimeItem)
    {
        RuntimeItem = &Equipment.DesiredMainHandItem;
        Definition = Cast<UBBBEquipmentDefinition>(RuntimeItem->Definition);
    }

    if (Equipment.TargetMode == EBBBEquipmentTargetMode::Mirror)
    {
        Definition = EquipmentCatalog.FindDefinition(Equipment.DesiredMirrorHandle);
    }

    if (!ensureMsgf(Definition, TEXT("[UBBBC]Equipment spawn aborted: desired item definition is not UBBBEquipmentDefinition")))
    {
        return;
    }

    if (!Definition->EquipmentActorClass
        || !CharacterMesh.GetOwner()
        || !CharacterMesh.GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("Equipment spawn requires actor class mesh and world"));
        return;
    }

    if (Equipment.TargetMode == EBBBEquipmentTargetMode::RuntimeItem
        && !ensureMsgf(RuntimeItem && RuntimeItem->RuntimeData, TEXT("[UBBBC]Equipment spawn aborted: runtime item data is null")))
    {
        return;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = CharacterMesh.GetOwner();
    SpawnParameters.Instigator = Cast<APawn>(CharacterMesh.GetOwner());
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABBBEquipmentActor *EquipmentActor = CharacterMesh.GetWorld()->SpawnActor<ABBBEquipmentActor>(
        Definition->EquipmentActorClass,
        FTransform::Identity,
        SpawnParameters);
    if (!EquipmentActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn requested equipment"));
        return;
    }

    EquipmentActor->AttachToComponent(
        &CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        RightHandWeaponSocketName);

    EquipmentActor->SetActorRelativeTransform(Definition->SpawnOffset);

    if (Equipment.TargetMode == EBBBEquipmentTargetMode::RuntimeItem)
    {
        EquipmentActor->InitializeRuntimeEquipment(*RuntimeItem, CharacterAPI);
    }

    if (Equipment.TargetMode == EBBBEquipmentTargetMode::Mirror)
    {
        EquipmentActor->InitializeEquipmentMirror(*Definition, CharacterAPI);
    }

    Equipment.EquippedItemActor = EquipmentActor;
    Equipment.ActiveTargetMode = Equipment.TargetMode;
    Equipment.ActiveMainHandDefinition = Definition;
    Equipment.ActiveMainHandItem = RuntimeItem ? *RuntimeItem : FBBBItemInstance();
    Equipment.ActiveMirrorHandle = Equipment.TargetMode == EBBBEquipmentTargetMode::Mirror
        ? Equipment.DesiredMirrorHandle
        : NAME_None;

    Equipment.bIsEquipping = Definition->EquipMontage != nullptr;

    Equipment.EquipEndTime = Equipment.bIsEquipping
        ? WorldTimeSeconds + Definition->EquipMontage->GetPlayLength()
        : 0.0f;

    EquipmentActor->Equip();
}
