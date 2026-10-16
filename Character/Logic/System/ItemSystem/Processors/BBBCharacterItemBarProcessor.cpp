#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemBarProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
void FBBBCharacterItemBarProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    const auto &Inventory = Context.RuntimeData.Item.ReadCharacterItemInventoryState();
    auto &Bar = Context.RuntimeData.Item.ItemBarState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    const int32 Start = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount;
    Bar.bStartUse = false;
    Bar.QuickAccessSlotCount = Inventory.QuickAccessSlotCount;
    bool bRecoverySelected = false;
    for (int32 Slot : Operations.PendingSelectedSlots)
    {
        ++Operations.Revision;
        const bool bEquipment = Slot >= 0 && Slot < Inventory.QuickAccessSlotCount;
        const bool bConsumable = Slot >= Start && Slot < Start + 5;
        if (Slot != INDEX_NONE && (!Inventory.Slots.IsValidIndex(Slot) || (!bEquipment && !bConsumable)))
        {
            ++Operations.RejectedCount;
            continue;
        }
        if (bConsumable)
        {
            Bar.SelectedConsumableSlot = Slot - Start;
            bRecoverySelected = Bar.SelectedConsumableSlot >= 2;
            if (!bRecoverySelected)
            {
                Bar.SelectedThrowableSlot = Bar.SelectedConsumableSlot;
            }
            Operations.bQuickUse |= Bar.SelectedConsumableSlot >= 2;
            Operations.UseStage = 0;
        }
        if (!bConsumable)
        {
            Bar.SelectedSlot = Slot;
        }
        ++Bar.Revision;
        ++Operations.SucceededCount;
    }
    Operations.PendingSelectedSlots.Reset();
    Operations.PendingFinishedInstances.Reset();
    Bar.DesiredMainHandItem = nullptr;
    if (Inventory.Slots.IsValidIndex(Bar.SelectedSlot))
    {
        const auto &Item = Inventory.Slots[Bar.SelectedSlot];
        if (Item.Definition && Item.Definition->ItemType == EBBBItemType::Equipment && Item.Quantity > 0 &&
            IsValid(Item.HeldInstance))
        {
            Bar.DesiredMainHandItem = Item.HeldInstance;
        }
    }
    if (!Bar.DesiredMainHandItem)
    {
        Bar.SelectedSlot = INDEX_NONE;
    }
    Bar.DesiredConsumableIdentity.Invalidate();
    Bar.DesiredConsumableDefinition = nullptr;
    const int32 UseSelection =
        Operations.bQuickUse && !bRecoverySelected ? Bar.SelectedThrowableSlot : Bar.SelectedConsumableSlot;
    const int32 Slot = Start + UseSelection;
    if (Inventory.Slots.IsValidIndex(Slot))
    {
        const auto &Item = Inventory.Slots[Slot];
        auto *Definition = Cast<UBBBConsumableDefinition>(Item.Definition);
        if (Definition && Item.Quantity > 0 && ((UseSelection < 2) == Definition->bThrowable))
        {
            Bar.DesiredConsumableIdentity = Item.InstanceId;
            Bar.DesiredConsumableDefinition = Definition;
        }
    }
    if (Operations.bQuickUse)
    {
        ++Operations.Revision;
        Bar.bStartUse = Bar.DesiredConsumableDefinition != nullptr;
        Bar.UseStage = Operations.UseStage;
        Operations.SucceededCount += Bar.bStartUse ? 1 : 0;
        Operations.RejectedCount += Bar.bStartUse ? 0 : 1;
    }
    Operations.bQuickUse = false;
}
void FBBBCharacterItemBarProcessor::Shutdown(FBBBCharacterItemUpdateContext &Context)
{
    auto &Bar = Context.RuntimeData.Item.ItemBarState;
    Bar.SelectedSlot = INDEX_NONE;
    Bar.DesiredMainHandItem = nullptr;
    Bar.DesiredConsumableDefinition = nullptr;
    Bar.DesiredConsumableIdentity.Invalidate();
    Bar.bStartUse = false;
    ++Bar.Revision;
}
