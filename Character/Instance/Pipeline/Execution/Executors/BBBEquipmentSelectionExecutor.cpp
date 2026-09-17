#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Execution/Executors/BBBEquipmentSelectionExecutor.h"

#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

void FBBBEquipmentSelectionExecutor::Update(
    FBBBDecisionRuntimeData &DecisionData,
    const FBBBCharacterEquipmentInventoryState &InventoryState,
    FBBBCharacterEquipmentState &EquipmentState) const
{
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForExecution(Index);

        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Approved)
        {
            continue;
        }

        if (Request.GetType() == EBBBCharacterActionType::Equip)
        {
            Request.MarkConsumed();

            const int32 QuickAccessSlot = Request.GetEquipSlot();
            if (!ensureMsgf(InventoryState.QuickAccessBindings.IsValidIndex(QuickAccessSlot), TEXT("[UBBBC]Requested quick access slot is out of range")))
            {
                continue;
            }

            ABBBEquipment *EquipmentInstance = InventoryState.QuickAccessBindings[QuickAccessSlot];
            if (!EquipmentInstance)
            {
                continue;
            }

            if (EquipmentState.DesiredMainHandInstance == EquipmentInstance)
            {
                continue;
            }

            EquipmentState.DesiredMainHandInstance = EquipmentInstance;
        }
    }
}
