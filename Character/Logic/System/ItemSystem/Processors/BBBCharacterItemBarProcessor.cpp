#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemBarProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionPrimaryLocalControlPacket.h"

void FBBBCharacterItemBarProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    const auto &Inventory = Context.RuntimeData.Item.ReadCharacterItemInventoryState();
    auto &Bar = Context.RuntimeData.Item.ItemBarState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    const int32 ConsumableStart = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount;
    Bar.QuickAccessSlotCount = Inventory.QuickAccessSlotCount;
    const int32 PreviousSlot = Bar.SelectedSlot;
    AActor *PreviousTarget = Bar.DesiredMainHandItem.Get();
    const auto RememberEquipment = [&Inventory, &Bar]()
    {
        if (Bar.SelectedSlot >= 0 && Bar.SelectedSlot < Inventory.QuickAccessSlotCount
            && Inventory.Slots.IsValidIndex(Bar.SelectedSlot) && Inventory.Slots[Bar.SelectedSlot].Quantity > 0)
        {
            Bar.PreviousEquipment = Inventory.Slots[Bar.SelectedSlot].InstanceId;
        }
    };
    const auto Restore = [&Inventory, &Bar]()
    {
        Bar.SelectedSlot = INDEX_NONE;
        for (int32 Slot = 0; Slot < Inventory.QuickAccessSlotCount; ++Slot)
        {
            if (Bar.PreviousEquipment.IsValid() && Inventory.Slots[Slot].InstanceId == Bar.PreviousEquipment
                && Inventory.Slots[Slot].Quantity > 0)
            {
                Bar.SelectedSlot = Slot;
                break;
            }
        }
        Bar.bReturnAfterUse = false;
        Bar.bUseWhenReady = false;
    };
    for (const FGuid &Instance : Operations.PendingFinishedInstances)
    {
        if (Inventory.Slots.IsValidIndex(Bar.SelectedSlot)
            && Inventory.Slots[Bar.SelectedSlot].InstanceId == Instance
            && (Bar.bReturnAfterUse || Inventory.Slots[Bar.SelectedSlot].Quantity <= 0))
        {
            Restore();
        }
    }
    Operations.PendingFinishedInstances.Reset();
    for (const int32 Slot : Operations.PendingSelectedSlots)
    {
        ++Operations.Revision;
        const bool bEquipment = Slot >= 0 && Slot < Inventory.QuickAccessSlotCount;
        const bool bConsumable = Slot >= ConsumableStart && Slot < ConsumableStart + Inventory.ConsumableQuickAccessSlotCount;
        if (Slot != INDEX_NONE && (!Inventory.Slots.IsValidIndex(Slot) || (!bEquipment && !bConsumable)))
        {
            ++Operations.RejectedCount;
            continue;
        }
        if (bConsumable)
        {
            RememberEquipment();
            Bar.SelectedConsumableSlot = Slot - ConsumableStart;
        }
        Bar.SelectedSlot = Slot;
        Bar.bReturnAfterUse = false;
        Bar.bUseWhenReady = false;
        ++Operations.SucceededCount;
    }
    Operations.PendingSelectedSlots.Reset();
    if (Operations.bQuickUse)
    {
        const int32 Slot = ConsumableStart + Bar.SelectedConsumableSlot;
        ++Operations.Revision;
        if (Inventory.Slots.IsValidIndex(Slot) && Inventory.Slots[Slot].Quantity > 0 && Inventory.Slots[Slot].HeldInstance)
        {
            RememberEquipment();
            Bar.SelectedSlot = Slot;
            Bar.bReturnAfterUse = true;
            Bar.bUseWhenReady = true;
            ++Operations.SucceededCount;
        }
        if (!Inventory.Slots.IsValidIndex(Slot) || Inventory.Slots[Slot].Quantity <= 0 || !Inventory.Slots[Slot].HeldInstance)
        {
            ++Operations.RejectedCount;
        }
    }
    Operations.bQuickUse = false;
    Bar.DesiredMainHandItem = nullptr;
    if (Inventory.Slots.IsValidIndex(Bar.SelectedSlot))
    {
        const auto &Item = Inventory.Slots[Bar.SelectedSlot];
        if (Item.Quantity > 0 && IsValid(Item.HeldInstance))
        {
            Bar.DesiredMainHandItem = Item.HeldInstance;
        }
        if (Item.Quantity == 0 && Item.HeldInstance && Context.RuntimeData.Interaction.ReadInteractionUseState().bBusy)
        {
            Bar.DesiredMainHandItem = Item.HeldInstance;
        }
    }
    if (Bar.bUseWhenReady && Bar.DesiredMainHandItem && Context.Character.GetHeldItem() == Bar.DesiredMainHandItem
        && Context.Character.IsItemUsable())
    {
        Context.Character.SubmitInput(FBBBCharacterInteractionPrimaryLocalControlPacket{});
        Bar.bUseWhenReady = false;
    }
    if (!Bar.DesiredMainHandItem && Bar.SelectedSlot >= 0 && Bar.SelectedSlot < Inventory.QuickAccessSlotCount)
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
    Bar.PreviousEquipment.Invalidate();
    Bar.bUseWhenReady = false;
    Bar.bReturnAfterUse = false;
    ++Bar.Revision;
}
