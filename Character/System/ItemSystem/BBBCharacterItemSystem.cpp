
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/BBBCharacterItemSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/World/BBBCharacterWorldRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void FBBBCharacterItemSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterItemRuntimeData &InItemData,
    FBBBCharacterExternalAPI &InCharacterAPI,
    UObject &InItemOuter,
    const FBBBCharacterEquipmentConfig &InEquipmentConfig,
    const FBBBCharacterItemConfig &InItemConfig,
    const FBBBCharacterWorldRuntimeData &InWorldData)
{
    CharacterMesh = &InCharacterMesh;
    ItemData = &InItemData;
    CharacterAPI = &InCharacterAPI;

    RightHandWeaponSocketName = InEquipmentConfig.RightHandWeaponSocketName;
    FBBBCharacterItemInventoryState &Inventory = ItemData->State.Inventory;
    Inventory.MainInventoryCapacity = FMath::Max(1, InItemConfig.MainInventoryCapacity);
    Inventory.HotbarCapacity = FMath::Max(1, InItemConfig.HotbarCapacity);
    Inventory.MainInventory.Entries.Reserve(Inventory.MainInventoryCapacity);
    Inventory.HotbarItemInstanceIds.Init(FGuid(), Inventory.HotbarCapacity);

    WorldData = &InWorldData;

    DefaultItemInitializer.Initialize(*ItemData, Storage, InItemOuter, InItemConfig);
}

void FBBBCharacterItemSystem::Update()
{

    if (!ensureMsgf(ItemData && WorldData && CharacterAPI && CharacterMesh && CharacterMesh->GetWorld(), TEXT("[UBBBC]Item system update aborted because dependencies are null")))
    {
        return;
    }

    FBBBCharacterEquipmentState &Equipment = ItemData->State.Equipment;

    EquipmentTransitionProcessor.Update(WorldData->GetWorldTimeSeconds(), Equipment);
    
    if (Equipment.ActiveMainHandItem.InstanceId != Equipment.DesiredMainHandItem.InstanceId)
    {

        WeaponSwitchProcessor.Update(Equipment);
        if (Equipment.DesiredMainHandItem.IsValid())
        {

            EquipmentSpawnProcessor.Update(
                *CharacterMesh,
                WorldData->GetWorldTimeSeconds(),
                RightHandWeaponSocketName,
                *ItemData,
                *CharacterAPI);
        }
    }

    ItemActionProcessor.Update(
        ItemData->Commands,
        ItemData->State.Equipment);
}
