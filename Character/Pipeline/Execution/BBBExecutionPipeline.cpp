#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/BBBExecutionPipeline.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemCommands.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemStates.h"

void FBBBExecutionPipeline::Initialize(
    FBBBDecisionRuntimeData &InDecisionData,
    FBBBCharacterItemCommands &InItemCommands,
    FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterBackpackState &InBackpackState)
{
    DecisionData = &InDecisionData;
    ItemCommands = &InItemCommands;
    EquipmentState = &InEquipmentState;
    BackpackState = &InBackpackState;
}

void FBBBExecutionPipeline::Update() const
{
    if (!ensureMsgf(
        DecisionData
            && ItemCommands
            && BackpackState
            && EquipmentState,
        TEXT("[UBBBC]Execution pipeline update failed because dependencies are null")))
    { return; }

    EquipmentExecutor.Update(
        *DecisionData,
        *BackpackState,
        *EquipmentState);

    ItemExecutor.Update(*DecisionData, *ItemCommands);
}
