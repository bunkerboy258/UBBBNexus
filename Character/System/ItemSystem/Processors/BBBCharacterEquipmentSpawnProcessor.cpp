
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterEquipmentSpawnProcessor.h"
#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentDefinition.h"

void FBBBCharacterEquipmentSpawnProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    float WorldTimeSeconds,
    FName RightHandWeaponSocketName,
    FBBBCharacterItemRuntimeData &ItemData,
    FBBBCharacterExternalAPI &CharacterAPI) const
{

    FBBBCharacterEquipmentState &Equipment = ItemData.State.Equipment;

    FBBBItemInstance &ItemInstance = Equipment.DesiredMainHandItem;

    UBBBEquipmentDefinition *Definition = Cast<UBBBEquipmentDefinition>(ItemInstance.Definition);
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

    if (!ensureMsgf(ItemInstance.RuntimeData, TEXT("[UBBBC]Equipment spawn aborted: item runtime data is null")))
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

    EquipmentActor->InitializeEquipment(ItemInstance, CharacterAPI);

    Equipment.EquippedItemActor = EquipmentActor;

    Equipment.ActiveMainHandItem = ItemInstance;

    Equipment.bIsEquipping = Definition->EquipMontage != nullptr;

    Equipment.EquipEndTime = Equipment.bIsEquipping
        ? WorldTimeSeconds + Definition->EquipMontage->GetPlayLength()
        : 0.0f;

    EquipmentActor->Equip();
}
