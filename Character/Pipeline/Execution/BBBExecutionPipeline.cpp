#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"

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
    if (!ensureMsgf(
        DecisionData
            && EquipmentCommands
            && InventoryState
            && EquipmentState,
        TEXT("[UBBBC]Execution pipeline update failed because dependencies are null")))
    { return; }

    SelectionExecutor.Update(
        *DecisionData,
        *InventoryState,
        *EquipmentState);

    ActionExecutor.Update(*DecisionData, *EquipmentCommands);
}
