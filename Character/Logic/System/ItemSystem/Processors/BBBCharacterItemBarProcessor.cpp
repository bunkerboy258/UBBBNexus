#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemBarProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBItemConfig.h"

void FBBBCharacterItemBarProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    const auto &Inventory = Context.RuntimeData.Item.ReadItemInventoryState();
    auto &Bar = Context.RuntimeData.Item.ItemBarState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    Bar.QuickAccessSlotCount = FMath::Clamp(Context.Config.QuickAccessSlotCount, 1, Inventory.BackpackSlotCount);
    const int32 PreviousSlot = Bar.SelectedSlot;
    AActor *PreviousTarget = Bar.DesiredMainHandItem.Get();
    for (const int32 Slot : Operations.PendingSelectedSlots)
    {
        ++Operations.Revision;
        if (Slot != INDEX_NONE && (Slot < 0 || Slot >= Bar.QuickAccessSlotCount
            || !Inventory.Slots.IsValidIndex(Slot)))
        {
            ++Operations.RejectedCount;
            continue;
        }
        if (Slot >= 0 && Inventory.Slots[Slot].Definition
            && Inventory.Slots[Slot].Definition->ItemType != EBBBItemType::Equipment)
        {
            ++Operations.RejectedCount;
            continue;
        }
        Bar.SelectedSlot = Slot;
        ++Operations.SucceededCount;
    }
    Operations.PendingSelectedSlots.Reset();

    Bar.DesiredMainHandItem = nullptr;
    if (Bar.SelectedSlot >= 0 && Bar.SelectedSlot < Bar.QuickAccessSlotCount
        && Inventory.Slots.IsValidIndex(Bar.SelectedSlot))
    {
        AActor *Item = Inventory.Slots[Bar.SelectedSlot].EquipmentInstance.Get();
        if (IsValid(Item))
        {
            Bar.DesiredMainHandItem = Item;
        }
    }
    if (!Bar.DesiredMainHandItem)
    {
        Bar.SelectedSlot = INDEX_NONE;
    }
    if (PreviousSlot != Bar.SelectedSlot || PreviousTarget != Bar.DesiredMainHandItem.Get())
    {
        ++Bar.Revision;
    }
}

void FBBBCharacterItemBarProcessor::Shutdown(FBBBCharacterItemUpdateContext &Context)
{
    auto &Bar = Context.RuntimeData.Item.ItemBarState;
    Bar.SelectedSlot = INDEX_NONE;
    Bar.DesiredMainHandItem = nullptr;
    ++Bar.Revision;
}
