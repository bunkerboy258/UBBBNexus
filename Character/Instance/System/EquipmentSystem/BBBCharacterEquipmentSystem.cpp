#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/BBBCharacterEquipmentSystem.h"

#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Equipment/BBBEquipmentConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterEquipmentRuntimeData &InEquipmentData,
    ABBBCharacter &InCharacter,
    const FBBBCharacterEquipmentConfig &InEquipmentConfig)
{
    // 保存角色装备系统依赖并按配置建立库存和快捷栏容量
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
    // 装备更新需要角色网格和装备运行时数据有效
    if (!ensureMsgf(EquipmentData && CharacterMesh && Character, TEXT("[UBBBC]Equipment system update dependencies are null")))
    {
        return;
    }

    // 先处理目标装备切换再处理当前装备动作
    SelectionProcessor.Update(
        *Character,
        *CharacterMesh,
        RightHandWeaponSocketName,
        EquipmentData->Commands,
        EquipmentData->Equipment,
        EquipmentData->Events);

    // 将本帧装备命令提交给当前装备外部接口
    ActionProcessor.Update(
        EquipmentData->Commands,
        EquipmentData->Equipment);
}
