#include "BBBWork/UBBBNexus/Character/Logic/Controller/EquipmentController/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/EquipmentController/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/EquipmentController/Processors/BBBCharacterEquipmentLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentCommandState.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(
    FBBBCharacterEquipmentUpdateContext &Context) const
{
    ABBBCharacter &Character = Context.Character;
    USkeletalMeshComponent &CharacterMesh = Context.CharacterMesh;
    FBBBCharacterEquipmentCommandState &EquipmentCommands = Context.CommandState;
    FBBBCharacterEquipmentSelectionState &EquipmentState = Context.SelectionState;

    // 先处理网络恢复的装备实例
    bool bRestoringEquipment = false;
    UBBBEquipmentDefinition *StateDefinition = EquipmentCommands.PendingEquipmentState;
    EquipmentCommands.PendingEquipmentState = nullptr;
    if (StateDefinition)
    {
        ABBBEquipment *StateInstance = FBBBCharacterEquipmentLifecycleProcessor::Create(
            Character,
            *StateDefinition,
            Context.bIsMirror);
        if (!StateInstance)
        {
            return;
        }

        // 恢复实例作为新的目标装备等待后续附着
        EquipmentState.DesiredMainHandInstance = StateInstance;
        bRestoringEquipment = true;
    }

    if (EquipmentState.ActiveMainHandInstance == EquipmentState.DesiredMainHandInstance)
    {
        return;
    }

    // 目标变化时先收束当前主手装备
    if (EquipmentState.ActiveMainHandInstance)
    {
        if (bRestoringEquipment)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Destroy(&CharacterMesh, *EquipmentState.ActiveMainHandInstance);
        }
        if (!bRestoringEquipment)
        {
            FBBBCharacterEquipmentLifecycleProcessor::Detach(&CharacterMesh, *EquipmentState.ActiveMainHandInstance);
        }
    }

    // 清除旧装备的换弹序号并切换当前实例引用
    EquipmentState.ActiveMainHandInstance = EquipmentState.DesiredMainHandInstance;
    ABBBEquipment *DesiredInstance = EquipmentState.ActiveMainHandInstance;
    EquipmentState.ActiveEquipmentId = DesiredInstance ? DesiredInstance->GetEquipmentId() : NAME_None;
    if (!DesiredInstance)
    {
        return;
    }

    // 附着失败时清理目标装备并回到未装备状态
    if (!FBBBCharacterEquipmentLifecycleProcessor::Attach(
        CharacterMesh,
        Context.RightHandWeaponSocketName,
        *DesiredInstance))
    {
        FBBBCharacterEquipmentLifecycleProcessor::Detach(&CharacterMesh, *DesiredInstance);
        EquipmentState.ActiveMainHandInstance = nullptr;
        EquipmentState.ActiveEquipmentId = NAME_None;
        return;
    }

    if (bRestoringEquipment)
    {
        return;
    }

    DesiredInstance->SubmitCommand(
        FBBBEquipmentCommand{
            EBBBEquipmentCommandType::Equip,
            EquipmentState.NextActionSequence++},
        Context.bIsMirror);
}
