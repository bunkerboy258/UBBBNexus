#include "BBBWork/UBBBNexus/Character/Pipeline/Execution/Executors/BBBEquipmentExecutor.h"

#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"

void FBBBEquipmentExecutor::Update(
    FBBBDecisionRuntimeData &DecisionData,
    const FBBBCharacterItemInventoryState &InventoryState,
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

            UBBBEquipmentInstance *ItemInstance = GetHotbarItem(InventoryState, Request.GetEquipSlot());
            if (!ensureMsgf(ItemInstance, TEXT("[UBBBC]Requested hotbar item is not valid equipment")))
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

UBBBEquipmentInstance *FBBBEquipmentExecutor::GetHotbarItem(
    const FBBBCharacterItemInventoryState &InventoryState,
    int32 HotbarSlot)
{
    if (HotbarSlot < 0 || HotbarSlot >= InventoryState.HotbarItemInstanceIds.Num())
    {
        return nullptr;
    }

    const FGuid &ItemInstanceId = InventoryState.HotbarItemInstanceIds[HotbarSlot];
    if (!ItemInstanceId.IsValid())
    {
        return nullptr;
    }

    for (const FBBBInventoryEntry &Entry : InventoryState.MainInventory.Entries)
    {
        if (Entry.ItemInstance && Entry.ItemInstance->GetInstanceId() == ItemInstanceId)
        {
            return Cast<UBBBEquipmentInstance>(Entry.ItemInstance);
        }
    }

    return nullptr;
}
