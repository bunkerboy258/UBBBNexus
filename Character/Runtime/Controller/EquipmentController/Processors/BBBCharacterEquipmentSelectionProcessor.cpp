#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentSelectionProcessor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterEquipmentSelectionProcessor::Update(
    ABBBCharacter &Character,
    USkeletalMeshComponent &CharacterMesh,
    const FName AttachmentSocketName,
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    FBBBCharacterEquipmentState &EquipmentState,
    FBBBCharacterEquipmentEvents &EquipmentEvents) const
{
    // 先处理网络恢复的装备实例
    bool bRestoringEquipment = false;
    UBBBEquipmentDefinition *RestoredDefinition = EquipmentCommands.ConsumeRestoredEquipment();
    if (RestoredDefinition)
    {
        ABBBEquipment *RestoredInstance = FBBBCharacterEquipmentLifecycleProcessor::Create(
            Character,
            *RestoredDefinition,
            true);
        if (!ensureMsgf(RestoredInstance, TEXT("[UBBBC]Restored equipment instance creation failed")))
        {
            return;
        }

        // 恢复实例作为新的目标装备等待后续附着
        EquipmentState.DesiredMainHandInstance = RestoredInstance;
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
    if (!FBBBCharacterEquipmentLifecycleProcessor::Attach(CharacterMesh, AttachmentSocketName, *DesiredInstance))
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

    // 普通装备切换完成后提交装备动作序号
    DesiredInstance->GetExternalAPI().SubmitEquip(EquipmentState.NextActionSequence++);
}
