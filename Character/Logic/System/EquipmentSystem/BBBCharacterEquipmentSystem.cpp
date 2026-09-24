#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/BBBCharacterEquipmentDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterRuntimeData &InRuntimeData,
    ABBBCharacter &InCharacter,
    const FBBBCharacterEquipmentConfig &InEquipmentConfig)
{
    // 保存角色装备系统依赖并按配置建立库存和快捷栏容量
    CharacterMesh = &InCharacterMesh;
    RuntimeData = &InRuntimeData;
    Character = &InCharacter;
    RightHandWeaponSocketName = InEquipmentConfig.RightHandWeaponSocketName;

    RuntimeData->Equipment.EquipmentInventoryState.BackpackSlots.Init(
        FBBBCharacterItem{},
        FMath::Max(1, InEquipmentConfig.InventorySlotCount));

    RuntimeData->Equipment.EquipmentInventoryState.ItemBarSlots.Init(
        FBBBCharacterItem{},
        FMath::Max(1, InEquipmentConfig.QuickAccessSlotCount));

}

void FBBBCharacterEquipmentSystem::Update()
{
    // 装备更新需要角色网格和装备运行时数据有效
    if (!RuntimeData || !CharacterMesh || !Character)
    {
        return;
    }

    FBBBCharacterEquipmentUpdateContext Context{
        *Character,
        *CharacterMesh,
        RightHandWeaponSocketName,
        RuntimeData->Equipment.EquipmentInventoryState,
        RuntimeData->Equipment.EquipmentSelectionState,
        RuntimeData->External.ReadNetworkIdentityState().bIsMirror};

    SelectionProcessor.Update(Context);
}

bool FBBBCharacterEquipmentSystem::RequestSlot(const int32 Slot)
{
    if (!RuntimeData || !IsInGameThread())
    {
        return false;
    }

    const auto &Inventory = RuntimeData->Equipment.ReadEquipmentInventoryState();
    if (Slot != INDEX_NONE && !ensureMsgf(Inventory.ItemBarSlots.IsValidIndex(Slot), TEXT("装备快捷栏索引无效")))
    {
        return false;
    }

    RuntimeData->Equipment.EquipmentSelectionState.PendingSlot = Slot;
    return true;
}

bool FBBBCharacterEquipmentSystem::RequestEquipment(const FName EquipmentId)
{
    if (!RuntimeData || !IsInGameThread())
    {
        return false;
    }

    auto &Selection = RuntimeData->Equipment.EquipmentSelectionState;
    Selection.PendingEquipmentId = EquipmentId;
    Selection.bHasEquipmentRequest = true;
    return true;
}
