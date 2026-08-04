#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBEquipmentExecutor.h"

#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/States/BBBCharacterItemStates.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentInstance.h"

void FBBBEquipmentExecutor::Update(
    FBBBDecisionRuntimeData &DecisionData,
    const FBBBCharacterBackpackState &BackpackState,
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
            if (!ensureMsgf(BackpackState.QuickAccessBindings.IsValidIndex(QuickAccessSlot), TEXT("[UBBBC]Requested quick access slot is out of range")))
            {
                continue;
            }

            UBBBEquipmentInstance *ItemInstance = Cast<UBBBEquipmentInstance>(BackpackState.QuickAccessBindings[QuickAccessSlot]);
            if (!ensureMsgf(ItemInstance, TEXT("[UBBBC]Requested quick access item is not valid equipment")))
            {
                continue;
            }

            if (EquipmentState.DesiredMainHandInstance == ItemInstance)
            {
                continue;
            }

            EquipmentState.DesiredMainHandInstance = ItemInstance;
        }
    }
}
