#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/System/BBBEquipmentSystem.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(
    USkeletalMeshComponent &CharacterMesh,
    FName AttachmentSocketName,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterExternalAPI &CharacterAPI) const
{
    // 期望与当前一致时 直接返回
    if (EquipmentState.ActiveMainHandInstance == EquipmentState.DesiredMainHandInstance)
    {
        return;
    }

    // 先取出期望装备所属的装备系统
    UBBBEquipmentSystem *DesiredEquipmentSystem = EquipmentState.DesiredMainHandInstance
        ? EquipmentState.DesiredMainHandInstance->GetEquipmentSystem()
        : nullptr;

    // 保证其装备系统有效
    if (EquipmentState.DesiredMainHandInstance
        && !ensureMsgf(DesiredEquipmentSystem, TEXT("[UBBBC]Desired equipment system is null")))
    {
        return;
    }

    // 若当前已经有主手装备，先释放旧装备表现
    if (EquipmentState.ActiveMainHandInstance)
    {
        UBBBEquipmentSystem *ActiveEquipmentSystem = EquipmentState.ActiveMainHandInstance->GetEquipmentSystem();
        if (!ensureMsgf(ActiveEquipmentSystem, TEXT("[UBBBC]Active equipment system is null")))
        {
            return;
        }

        ActiveEquipmentSystem->ReleasePresentation();
    }

    // 无论是否切换到新装备 都先把当前主手实例同步成期望主手实例
    EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;

    // 如果存在新主手装备 再成实际装备动作
    if (EquipmentState.ActiveMainHandInstance)
    {
        DesiredEquipmentSystem->Equip(
            CharacterMesh,
            CharacterAPI,
            AttachmentSocketName);
    }
}
