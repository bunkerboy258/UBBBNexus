#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"

void FBBBExecutionPipeline::Initialize(
    FBBBDecisionRuntimeData &InDecisionData,
    FBBBCharacterEquipmentCommands &InEquipmentCommands,
    FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterEquipmentInventoryState &InInventoryState)
{
    DecisionData = &InDecisionData;
    EquipmentCommands = &InEquipmentCommands;
    EquipmentState = &InEquipmentState;
    InventoryState = &InInventoryState;
}

void FBBBExecutionPipeline::Update() const
{
    // 执行阶段需要仲裁结果装备命令和装备状态完整有效
    if (!ensureMsgf(
        DecisionData
            && EquipmentCommands
            && InventoryState
            && EquipmentState,
        TEXT("[UBBBC]Execution pipeline update failed because dependencies are null")))
    { return; }

    // 将换弹通知移交给装备命令完成输入所有权转移
    EquipmentCommands->ReloadInputs = MoveTemp(DecisionData->ReloadInputs);

    // 先执行装备选择再执行开火和换弹动作
    SelectionExecutor.Update(
        *DecisionData,
        *InventoryState,
        *EquipmentState);

    // 将批准请求转换为装备系统命令
    ActionExecutor.Update(*DecisionData, *EquipmentCommands);
}
