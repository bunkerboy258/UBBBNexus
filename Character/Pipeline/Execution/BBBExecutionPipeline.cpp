#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemCommands.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"

void FBBBExecutionPipeline::Initialize(
    FBBBDecisionRuntimeData &InDecisionData,
    FBBBCharacterItemCommands &InItemCommands,
    FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterItemInventoryState &InInventoryState)
{
    DecisionData = &InDecisionData;
    ItemCommands = &InItemCommands;
    EquipmentState = &InEquipmentState;
    InventoryState = &InInventoryState;
}

void FBBBExecutionPipeline::Update() const
{
    if (!ensureMsgf(
        DecisionData
            && ItemCommands
            && InventoryState
            && EquipmentState,
        TEXT("[UBBBC]Execution pipeline update failed because dependencies are null")))
    {
        return;
    }

    EquipmentExecutor.Update(
        *DecisionData,
        *InventoryState,
        *EquipmentState);

    ItemExecutor.Update(*DecisionData, *ItemCommands);
}
