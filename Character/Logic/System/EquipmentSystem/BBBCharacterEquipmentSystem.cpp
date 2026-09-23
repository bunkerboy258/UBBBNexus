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
        RuntimeData->Equipment.EquipmentCommandState,
        RuntimeData->External.ReadNetworkIdentityState().bIsMirror};

    SelectionProcessor.Update(Context);
    ActionProcessor.Update(Context);
}
