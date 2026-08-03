
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
    EquipmentCatalog = InItemConfig.EquipmentCatalog;
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

    if (!ensureMsgf(ItemData && WorldData && CharacterAPI && CharacterMesh && EquipmentCatalog && CharacterMesh->GetWorld(), TEXT("[UBBBC]Item system update aborted because dependencies are null")))
    { return; }

    FBBBCharacterEquipmentState &Equipment = ItemData->State.Equipment;

    //推进装备的过渡状态
    EquipmentTransitionProcessor.Update(WorldData->GetWorldTimeSeconds(), Equipment);
    
    bool bRequiresEquipmentChange = false;

    if (Equipment.TargetMode == EBBBEquipmentTargetMode::RuntimeItem)
    {
        bRequiresEquipmentChange = Equipment.ActiveTargetMode != EBBBEquipmentTargetMode::RuntimeItem
            || Equipment.ActiveMainHandItem.InstanceId != Equipment.DesiredMainHandItem.InstanceId;
    }

    if (Equipment.TargetMode == EBBBEquipmentTargetMode::Mirror)
    {
        bRequiresEquipmentChange = Equipment.ActiveTargetMode != EBBBEquipmentTargetMode::Mirror
            || Equipment.ActiveMirrorHandle != Equipment.DesiredMirrorHandle;
    }

    if (Equipment.TargetMode == EBBBEquipmentTargetMode::None)
    {
        bRequiresEquipmentChange = Equipment.EquippedItemActor != nullptr;
    }

    if (bRequiresEquipmentChange)
    {

        WeaponSwitchProcessor.Update(Equipment);
        if (Equipment.TargetMode == EBBBEquipmentTargetMode::RuntimeItem
            && Equipment.DesiredMainHandItem.IsValid())
        {

            EquipmentSpawnProcessor.Update(
                *CharacterMesh,
                WorldData->GetWorldTimeSeconds(),
                RightHandWeaponSocketName,
                *EquipmentCatalog,
                *ItemData,
                *CharacterAPI);
        }

        if (Equipment.TargetMode == EBBBEquipmentTargetMode::Mirror
            && !Equipment.DesiredMirrorHandle.IsNone())
        {
            EquipmentSpawnProcessor.Update(
                *CharacterMesh,
                WorldData->GetWorldTimeSeconds(),
                RightHandWeaponSocketName,
                *EquipmentCatalog,
                *ItemData,
                *CharacterAPI);
        }
    }

    ItemActionProcessor.Update(
        ItemData->Commands,
        ItemData->State.Equipment,
        ItemData->ActionResults);
}
