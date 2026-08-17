#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterEquipmentRuntimeData &InEquipmentData,
    FBBBCharacterExternalAPI &InCharacterAPI,
    UObject &InEquipmentOuter,
    const FBBBCharacterEquipmentConfig &InEquipmentConfig)
{
    CharacterMesh = &InCharacterMesh;
    EquipmentData = &InEquipmentData;
    CharacterAPI = &InCharacterAPI;
    EquipmentOuter = &InEquipmentOuter;
    RightHandWeaponSocketName = InEquipmentConfig.RightHandWeaponSocketName;

    EquipmentData->Inventory.Slots.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.InventorySlotCount));

    EquipmentData->Inventory.QuickAccessBindings.Init(
        nullptr,
        FMath::Max(1, InEquipmentConfig.QuickAccessSlotCount));

    DefaultEquipmentInitializer.Initialize(
        *EquipmentData,
        InEquipmentOuter,
        InEquipmentConfig);
}

void FBBBCharacterEquipmentSystem::Update()
{
    if (!ensureMsgf(EquipmentData && CharacterAPI && CharacterMesh && EquipmentOuter, TEXT("[UBBBC]Equipment system update dependencies are null")))
    {
        return;
    }

    SelectionProcessor.Update(
        *CharacterMesh,
        RightHandWeaponSocketName,
        *EquipmentOuter,
        EquipmentData->Commands,
        EquipmentData->Equipment,
        *CharacterAPI);

    ActionProcessor.Update(
        EquipmentData->Commands,
        EquipmentData->Equipment,
        EquipmentData->Results,
        *CharacterAPI);
}
