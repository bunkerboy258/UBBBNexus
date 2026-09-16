#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacterInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterEquipmentRuntimeData &InEquipmentData,
    ABBBCharacterInstance &InCharacter,
    const FBBBCharacterEquipmentConfig &InEquipmentConfig)
{
    CharacterMesh = &InCharacterMesh;
    EquipmentData = &InEquipmentData;
    Character = &InCharacter;
    RightHandWeaponSocketName = InEquipmentConfig.RightHandWeaponSocketName;

    EquipmentData->Inventory.Slots.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.InventorySlotCount));

    EquipmentData->Inventory.QuickAccessBindings.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.QuickAccessSlotCount));

}

void FBBBCharacterEquipmentSystem::Update()
{
    if (!ensureMsgf(EquipmentData && CharacterMesh && Character, TEXT("[UBBBC]Equipment system update dependencies are null")))
    {
        return;
    }

    SelectionProcessor.Update(
        *Character,
        *CharacterMesh,
        RightHandWeaponSocketName,
        EquipmentData->Commands,
        EquipmentData->Equipment,
        EquipmentData->Events);

    ActionProcessor.Update(
        EquipmentData->Commands,
        EquipmentData->Equipment);
}
