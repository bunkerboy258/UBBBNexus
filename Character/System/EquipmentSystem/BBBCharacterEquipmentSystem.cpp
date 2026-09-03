#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterEquipmentRuntimeData &InEquipmentData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    ABBBCharacter &InCharacter,
    const FBBBCharacterEquipmentConfig &InEquipmentConfig)
{
    CharacterMesh = &InCharacterMesh;
    EquipmentData = &InEquipmentData;
    WorldData = &InWorldData;
    Character = &InCharacter;
    RightHandWeaponSocketName = InEquipmentConfig.RightHandWeaponSocketName;

    EquipmentData->Inventory.Slots.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.InventorySlotCount));

    EquipmentData->Inventory.QuickAccessBindings.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.QuickAccessSlotCount));

    DefaultEquipmentInitializer.Initialize(
        *EquipmentData,
        InCharacter,
        InEquipmentConfig);
}

void FBBBCharacterEquipmentSystem::Update()
{
    if (!ensureMsgf(EquipmentData && WorldData && CharacterMesh && Character, TEXT("[UBBBC]Equipment system update dependencies are null")))
    {
        return;
    }

    SelectionProcessor.Update(
        *Character,
        *CharacterMesh,
        RightHandWeaponSocketName,
        WorldData->GetWorldTimeSeconds(),
        EquipmentData->Commands,
        EquipmentData->Equipment,
        EquipmentData->Events);

    ActionProcessor.Update(
        WorldData->GetWorldTimeSeconds(),
        EquipmentData->Commands,
        EquipmentData->Equipment,
        EquipmentData->Events);
}
