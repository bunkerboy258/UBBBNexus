#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemLifecycleProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"

void FBBBCharacterItemLifecycleProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    for (auto &Item : Inventory.Slots)
    {
        if (!Item.Definition)
        {
            continue;
        }
        if (!IsValid(Item.Definition) || !Item.InstanceId.IsValid()
            || ((Item.Definition->ItemType == EBBBItemType::Equipment
                || Item.Definition->ItemType == EBBBItemType::Consumable) && !IsValid(Item.HeldInstance)))
        {
            Item.Quantity = 0;
        }
        const auto *Consumable = Cast<ABBBConsumable>(Item.HeldInstance);
        if (Item.Quantity > 0 || (Item.HeldInstance && Item.HeldInstance == Context.Character.GetHeldItem())
            || (Consumable && Consumable->IsUsing()))
        {
            continue;
        }
        if (IsValid(Item.HeldInstance))
        {
            Item.HeldInstance->Destroy();
        }
        Item = {};
        ++Inventory.Revision;
    }
}

void FBBBCharacterItemLifecycleProcessor::Shutdown(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    for (auto &Item : Inventory.Slots)
    {
        if (IsValid(Item.HeldInstance))
        {
            Item.HeldInstance->Destroy();
        }
    }
    Inventory.Slots.Reset();
    Inventory.WearSlots.Reset();
    Inventory.BackpackSlotCount = 0;
    ++Inventory.Revision;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    Operations.PendingItemIds.Reset();
    Operations.PendingQuantities.Reset();
    Operations.PendingMoveSources.Reset();
    Operations.PendingMoveTargets.Reset();
    Operations.PendingMoveInstances.Reset();
    Operations.PendingSelectedSlots.Reset();
    Operations.PendingConsumeInstances.Reset();
    Operations.PendingConsumeQuantities.Reset();
    Operations.PendingFinishedInstances.Reset();
    Operations.bQuickUse = false;
    Operations.Revision = 0;
    Operations.SucceededCount = 0;
    Operations.RejectedCount = 0;
}
