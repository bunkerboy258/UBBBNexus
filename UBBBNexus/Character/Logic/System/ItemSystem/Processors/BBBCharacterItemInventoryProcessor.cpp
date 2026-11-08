#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemInventoryProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"

void FBBBCharacterItemInventoryProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    if (Inventory.Slots.IsEmpty())
    {
        Inventory.QuickAccessSlotCount = 5;
        Inventory.ConsumableQuickAccessSlotCount = 5;
        Inventory.EquipmentSlotCount = FMath::Max(Inventory.QuickAccessSlotCount, Context.Config.EquipmentSlotCount);
        Inventory.WearableSlotCount = FMath::Max(0, Context.Config.WearableSlotCount);
        Inventory.ConsumableSlotCount = FMath::Max(Inventory.ConsumableQuickAccessSlotCount, Context.Config.ConsumableSlotCount);
        Inventory.MiscSlotCount = FMath::Max(0, Context.Config.MiscSlotCount);
        Inventory.BackpackSlotCount = Inventory.EquipmentSlotCount + Inventory.WearableSlotCount
            + Inventory.ConsumableSlotCount + Inventory.MiscSlotCount;
        TSet<FName> Unique;
        for (FName Slot : Context.Config.WearSlots)
        {
            if (ensureMsgf(!Slot.IsNone() && !Unique.Contains(Slot), TEXT("穿戴位置必须非空且唯一")))
            {
                Unique.Add(Slot);
                Inventory.WearSlots.Add(Slot);
            }
        }
        Inventory.Slots.SetNum(Inventory.BackpackSlotCount + Inventory.WearSlots.Num());
        ++Inventory.Revision;
    }
    if (!Operations.PendingItemIds.IsEmpty() || !Operations.PendingMoveSources.IsEmpty()
        || !Operations.PendingSelectedSlots.IsEmpty() || !Operations.PendingConsumeInstances.IsEmpty()
        || !Operations.PendingFinishedInstances.IsEmpty() || Operations.bQuickUse)
    {
        Operations.SucceededCount = 0;
        Operations.RejectedCount = 0;
    }
    const auto Allows = [&Inventory](int32 Slot, const FBBBCharacterItem &Item)
    {
        if (!Inventory.Slots.IsValidIndex(Slot))
        {
            return false;
        }
        if (!Item.Definition)
        {
            return true;
        }
        const auto Type = Item.Definition->ItemType;
        if (Slot < Inventory.EquipmentSlotCount)
        {
            return Type == EBBBItemType::Equipment;
        }
        if (Slot < Inventory.EquipmentSlotCount + Inventory.WearableSlotCount)
        {
            return Type == EBBBItemType::Wearable;
        }
        if (Slot < Inventory.EquipmentSlotCount + Inventory.WearableSlotCount + Inventory.ConsumableSlotCount)
        {
            const int32 Index = Slot - Inventory.EquipmentSlotCount - Inventory.WearableSlotCount;
            const auto *Definition = Cast<UBBBConsumableDefinition>(Item.Definition);
            return Type == EBBBItemType::Consumable && Definition &&
                   (Index >= 5 || ((Index < 2) == Definition->bThrowable));
        }
        if (Slot < Inventory.BackpackSlotCount)
        {
            return Type == EBBBItemType::Misc;
        }
        return Type == EBBBItemType::Wearable
            && Item.Definition->WearSlot == Inventory.WearSlots[Slot - Inventory.BackpackSlotCount];
    };
    for (int32 Index = 0; Index < Operations.PendingConsumeInstances.Num(); ++Index)
    {
        ++Operations.Revision;
        FBBBCharacterItem *Item = Inventory.Slots.FindByPredicate([&Operations, Index](const auto &Value)
        {
            return Value.InstanceId == Operations.PendingConsumeInstances[Index];
        });
        const int32 Quantity = Operations.PendingConsumeQuantities[Index];
        if (!Item || !Item->Definition || Item->Definition->ItemType != EBBBItemType::Consumable
            || Quantity <= 0 || Item->Quantity < Quantity)
        {
            ++Operations.RejectedCount;
            continue;
        }
        Item->Quantity -= Quantity;
        ++Inventory.Revision;
        ++Operations.SucceededCount;
    }
    Operations.PendingConsumeInstances.Reset();
    Operations.PendingConsumeQuantities.Reset();
    for (int32 Index = 0; Index < Operations.PendingMoveSources.Num(); ++Index)
    {
        const int32 Source = Operations.PendingMoveSources[Index];
        int32 Target = Operations.PendingMoveTargets[Index];
        ++Operations.Revision;
        if (!Inventory.Slots.IsValidIndex(Source) || !Inventory.Slots[Source].Definition
            || Inventory.Slots[Source].Quantity <= 0
            || Inventory.Slots[Source].InstanceId != Operations.PendingMoveInstances[Index]
            || Context.RuntimeData.Interaction.ReadInteractionUseState().bBusy)
        {
            ++Operations.RejectedCount;
            continue;
        }
        if (Target == INDEX_NONE)
        {
            for (int32 Slot = 0; Slot < Inventory.BackpackSlotCount; ++Slot)
            {
                if (!Inventory.Slots[Slot].Definition && Allows(Slot, Inventory.Slots[Source]))
                {
                    Target = Slot;
                    break;
                }
            }
        }
        if (!Allows(Target, Inventory.Slots[Source]) || !Allows(Source, Inventory.Slots[Target]))
        {
            ++Operations.RejectedCount;
            continue;
        }
        auto &From = Inventory.Slots[Source];
        auto &To = Inventory.Slots[Target];
        if (Source != Target && From.Definition == To.Definition && From.Definition->MaxStack > 1)
        {
            const int32 Added = FMath::Min(From.Quantity, From.Definition->MaxStack - To.Quantity);
            if (Added <= 0)
            {
                ++Operations.RejectedCount;
                continue;
            }
            From.Quantity -= Added;
            To.Quantity += Added;
            ++Inventory.Revision;
        }
        if (Source != Target && !(From.Definition == To.Definition && From.Definition->MaxStack > 1))
        {
            Swap(From, To);
            ++Inventory.Revision;
        }
        ++Operations.SucceededCount;
    }
    Operations.PendingMoveSources.Reset();
    Operations.PendingMoveTargets.Reset();
    Operations.PendingMoveInstances.Reset();
}
