#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemBarProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
void FBBBCharacterItemBarProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    const auto &Inventory = Context.RuntimeData.Item.ReadCharacterItemInventoryState();
    auto &Bar = Context.RuntimeData.Item.ItemBarState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    const int32 Start = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount;
    const int32 ConsumableCount = Inventory.ConsumableQuickAccessSlotCount;
    Bar.bStartUse = false;
    if (!Bar.bInitializedPlayerWeaponSelection && Inventory.QuickAccessSlotCount > 0 && Context.Character.IsPlayerControlled())
    {
        Bar.bInitializedPlayerWeaponSelection = true;
        Bar.SelectedSlot = 0;
    }
    Bar.QuickAccessSlotCount = Inventory.QuickAccessSlotCount;
    bool bRecoverySelected = false;
    for (int32 Slot : Operations.PendingSelectedSlots)
    {
        ++Operations.Revision;
        const bool bEquipment = Slot >= 0 && Slot < Inventory.QuickAccessSlotCount;
        const bool bConsumable = Slot >= Start && Slot < Start + ConsumableCount;
        if (Slot != INDEX_NONE && (!Inventory.Slots.IsValidIndex(Slot) || (!bEquipment && !bConsumable)))
        {
            ++Operations.RejectedCount;
            continue;
        }
        if (bConsumable)
        {
            const auto &Item = Inventory.Slots[Slot];
            const auto *Definition = Cast<UBBBConsumableDefinition>(Item.Definition);
            if (Definition && Item.Quantity <= 0)
            {
                ++Operations.RejectedCount;
                continue;
            }
            Bar.SelectedConsumableSlot = Slot - Start;
            bRecoverySelected = Definition && !Definition->bThrowable;
            if (Definition && Definition->bThrowable)
            {
                Bar.SelectedThrowableSlot = Bar.SelectedConsumableSlot;
            }
            Operations.bQuickUse |= bRecoverySelected;
        }
        if (!bConsumable)
        {
            Bar.SelectedSlot = Slot;
        }
        ++Bar.Revision;
        ++Operations.SucceededCount;
    }
    const auto &Use = Context.RuntimeData.Item.ReadCharacterItemUseState();
    const bool bUseBusy = Context.RuntimeData.Interaction.ReadInteractionUseState().bBusy;
    const auto FindNextAfterExhaustion = [&](int32 Selection, bool bThrowableOnly) -> int32
    {
        if (Selection < 0 || Selection >= ConsumableCount)
        {
            return Selection;
        }
        const auto &Current = Inventory.Slots[Start + Selection];
        const auto *CurrentDefinition = Cast<UBBBConsumableDefinition>(Current.Definition);
        if (!CurrentDefinition || Current.Quantity != 0 || !Current.InstanceId.IsValid()
            || (!Operations.PendingFinishedInstances.Contains(Current.InstanceId)
                && (bUseBusy || Use.Instance != Current.InstanceId)))
        {
            return Selection;
        }
        for (int32 Step = 1; Step < ConsumableCount; ++Step)
        {
            const int32 Candidate = (Selection + Step) % ConsumableCount;
            const auto &Item = Inventory.Slots[Start + Candidate];
            const auto *Definition = Cast<UBBBConsumableDefinition>(Item.Definition);
            if (Definition && IsValid(Definition->RuntimeClass.Get()) && Item.InstanceId.IsValid()
                && Item.Quantity > 0 && Definition->bThrowable == (bThrowableOnly || CurrentDefinition->bThrowable))
            {
                return Candidate;
            }
        }
        return INDEX_NONE;
    };
    const int32 NextThrowable = FindNextAfterExhaustion(Bar.SelectedThrowableSlot, true);
    const int32 NextConsumable = FindNextAfterExhaustion(Bar.SelectedConsumableSlot, false);
    const bool bThrowableAdvanced = NextThrowable != Bar.SelectedThrowableSlot;
    if (bThrowableAdvanced || NextConsumable != Bar.SelectedConsumableSlot)
    {
        Bar.SelectedThrowableSlot = NextThrowable;
        Bar.SelectedConsumableSlot = NextConsumable;
        ++Bar.Revision;
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
    if (UseSelection >= 0 && UseSelection < Inventory.ConsumableQuickAccessSlotCount && Inventory.Slots.IsValidIndex(Slot))
    {
        const auto &Item = Inventory.Slots[Slot];
        auto *Definition = Cast<UBBBConsumableDefinition>(Item.Definition);
        if (Definition && Item.Quantity > 0 && (!Operations.bQuickUse || bRecoverySelected || Definition->bThrowable))
        {
            Bar.DesiredConsumableIdentity = Item.InstanceId;
            Bar.DesiredConsumableDefinition = Definition;
        }
    }
    if (Operations.bQuickUse)
    {
        ++Operations.Revision;
        Bar.bStartUse = Bar.DesiredConsumableDefinition != nullptr && (!bThrowableAdvanced || bRecoverySelected);
        Operations.SucceededCount += Bar.bStartUse ? 1 : 0;
        Operations.RejectedCount += Bar.bStartUse ? 0 : 1;
    }
    Operations.bQuickUse = false;
}
void FBBBCharacterItemBarProcessor::Shutdown(FBBBCharacterItemUpdateContext &Context)
{
    auto &Bar = Context.RuntimeData.Item.ItemBarState;
    Bar.QuickAccessSlotCount = 0;
    Bar.bInitializedPlayerWeaponSelection = false;
    Bar.SelectedSlot = INDEX_NONE;
    Bar.SelectedConsumableSlot = 0;
    Bar.SelectedThrowableSlot = 0;
    Bar.DesiredMainHandItem = nullptr;
    Bar.DesiredConsumableDefinition = nullptr;
    Bar.DesiredConsumableIdentity.Invalidate();
    Bar.bStartUse = false;
    ++Bar.Revision;
}
