#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/Processors/BBBCharacterItemInventoryProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Context/BBBCharacterItemUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBItemConfig.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

void FBBBCharacterItemInventoryProcessor::Update(FBBBCharacterItemUpdateContext &Context) const
{
    auto &Inventory = Context.RuntimeData.Item.ItemInventoryState;
    auto &Operations = Context.RuntimeData.Item.ItemOperationState;
    if (Inventory.Slots.IsEmpty())
    {
        Inventory.QuickAccessSlotCount = FMath::Max(1, Context.Config.QuickAccessSlotCount);
        Inventory.EquipmentStorageSlotCount = FMath::Max(0, Context.Config.EquipmentStorageSlotCount);
        Inventory.BackpackSlotCount = Inventory.QuickAccessSlotCount + Inventory.EquipmentStorageSlotCount
            + FMath::Max(0, Context.Config.MiscStorageSlotCount);
        TSet<FName> UniqueSlots;
        for (const FName Name : Context.Config.WearSlots)
        {
            if (!ensureMsgf(!Name.IsNone() && !UniqueSlots.Contains(Name), TEXT("穿戴位置必须非空且唯一")))
            {
                continue;
            }
            UniqueSlots.Add(Name);
            Inventory.WearSlots.Add(Name);
        }
        Inventory.Slots.SetNum(Inventory.BackpackSlotCount + Inventory.WearSlots.Num());
        ++Inventory.Revision;
    }
    if (!Operations.PendingItemIds.IsEmpty() || !Operations.PendingMoveSources.IsEmpty()
        || !Operations.PendingSelectedSlots.IsEmpty())
    {
        Operations.SucceededCount = 0;
        Operations.RejectedCount = 0;
    }
    for (FBBBCharacterItem &Item : Inventory.Slots)
    {
        if (Item.Definition && (!IsValid(Item.Definition) || !Item.InstanceId.IsValid()
            || (Item.Definition->ItemType == EBBBItemType::Equipment && !IsValid(Item.EquipmentInstance))))
        {
            Item.InstanceId.Invalidate();
            Item.Definition = nullptr;
            Item.EquipmentInstance = nullptr;
            ++Inventory.Revision;
        }
    }
    const auto Allows = [&Inventory](const int32 Slot, const FBBBCharacterItem &Item)
    {
        if (!Inventory.Slots.IsValidIndex(Slot))
        {
            return false;
        }
        if (!Item.Definition)
        {
            return true;
        }
        if (Slot < Inventory.BackpackSlotCount)
        {
            const bool bMiscRegion = Slot >= Inventory.QuickAccessSlotCount + Inventory.EquipmentStorageSlotCount;
            return bMiscRegion == (Item.Definition->ItemType == EBBBItemType::Misc);
        }
        return Item.Definition->ItemType != EBBBItemType::Equipment
            && Item.Definition->WearSlot == Inventory.WearSlots[Slot - Inventory.BackpackSlotCount];
    };
    for (int32 Index = 0; Index < Operations.PendingMoveSources.Num(); ++Index)
    {
        const int32 Source = Operations.PendingMoveSources[Index];
        int32 Target = Operations.PendingMoveTargets.IsValidIndex(Index)
            ? Operations.PendingMoveTargets[Index] : INDEX_NONE;
        if (Target == INDEX_NONE)
        {
            for (int32 Slot = 0; Slot < Inventory.BackpackSlotCount; ++Slot)
            {
                if (Inventory.Slots.IsValidIndex(Source) && !Inventory.Slots[Slot].Definition
                    && Allows(Slot, Inventory.Slots[Source]))
                {
                    Target = Slot;
                    break;
                }
            }
        }
        ++Operations.Revision;
        if (!Operations.PendingMoveInstances.IsValidIndex(Index)
            || !Inventory.Slots.IsValidIndex(Source)
            || Inventory.Slots[Source].InstanceId != Operations.PendingMoveInstances[Index] || !Inventory.Slots.IsValidIndex(Target)
            || !Inventory.Slots[Source].Definition || !Allows(Target, Inventory.Slots[Source])
            || !Allows(Source, Inventory.Slots[Target]))
        {
            ++Operations.RejectedCount;
            continue;
        }
        if (Source != Target)
        {
            Swap(Inventory.Slots[Source], Inventory.Slots[Target]);
            ++Inventory.Revision;
        }
        ++Operations.SucceededCount;
    }
    Operations.PendingMoveSources.Reset();
    Operations.PendingMoveTargets.Reset();
    Operations.PendingMoveInstances.Reset();
}
