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
        *RuntimeData,
        RightHandWeaponSocketName,
        RuntimeData->External.ReadNetworkIdentityState().bIsMirror};

    SelectionProcessor.Update(Context);
    LifecycleProcessor.Update(Context);
}

void FBBBCharacterEquipmentSystem::Shutdown()
{
    if (!RuntimeData || !CharacterMesh || !Character)
    {
        return;
    }
    FBBBCharacterEquipmentUpdateContext Context{
        *Character, *CharacterMesh, *RuntimeData, RightHandWeaponSocketName,
        RuntimeData->External.ReadNetworkIdentityState().bIsMirror};
    LifecycleProcessor.Shutdown(Context);
}

void FBBBCharacterEquipmentSystem::UpdateUsage()
{
    if (!RuntimeData || !CharacterMesh || !Character)
    {
        return;
    }

    FBBBCharacterEquipmentUpdateContext Context{
        *Character,
        *CharacterMesh,
        *RuntimeData,
        RightHandWeaponSocketName,
        RuntimeData->External.ReadNetworkIdentityState().bIsMirror};
    UseProcessor.Update(Context);
    ActionPermissionProcessor.Update(Context);
    AnimationInputProcessor.Update(Context);
}
