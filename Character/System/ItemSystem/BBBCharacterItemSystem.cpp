#include "BBBWork/UBBBNexus/Character/System/ItemSystem/BBBCharacterItemSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/BBBCharacterConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Context/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

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
    WorldData = &InWorldData;
    RightHandWeaponSocketName = InEquipmentConfig.RightHandWeaponSocketName;

    FBBBCharacterBackpackState &Backpack = ItemData->Backpack;
    Backpack.Slots.Init(nullptr, FMath::Max(1, InItemConfig.BackpackSlotCount));
    Backpack.QuickAccessBindings.Init(nullptr, FMath::Max(1, InItemConfig.QuickAccessSlotCount));

    DefaultItemInitializer.Initialize(*ItemData, Storage, InItemOuter, InItemConfig);
}

void FBBBCharacterItemSystem::Update()
{
    if (!ensureMsgf(ItemData && WorldData && CharacterAPI && CharacterMesh, TEXT("[UBBBC]Item system update dependencies are null")))
    { return; }

    EquipmentProcessor.Update(
        *CharacterMesh,
        WorldData->GetWorldTimeSeconds(),
        RightHandWeaponSocketName,
        ItemData->Equipment,
        *CharacterAPI);

    ItemActionProcessor.Update(
        ItemData->Commands,
        ItemData->Equipment,
        ItemData->ActionResults);
}
