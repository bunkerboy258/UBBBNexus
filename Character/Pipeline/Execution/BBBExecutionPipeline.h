#pragma once
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBEquipmentExecutor.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBItemActionExecutor.h"
class FBBBCharacterInitializer;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemCommands;
struct FBBBCharacterItemInventoryState;
struct FBBBDecisionRuntimeData;

//执行所有批准请求
class FBBBExecutionPipeline final
{
public:

    void Update() const;

private:

    friend class FBBBCharacterInitializer;

    void Initialize(
        FBBBDecisionRuntimeData &InDecisionData,
        FBBBCharacterItemCommands &InItemCommands,
        FBBBCharacterEquipmentState &InEquipmentState,
        const FBBBCharacterItemInventoryState &InInventoryState);

    FBBBDecisionRuntimeData *DecisionData = nullptr;

    FBBBCharacterItemCommands *ItemCommands = nullptr;

    const FBBBCharacterItemInventoryState *InventoryState = nullptr;

    FBBBCharacterEquipmentState *EquipmentState = nullptr;

    FBBBEquipmentExecutor EquipmentExecutor;

    FBBBItemActionExecutor ItemExecutor;
};
