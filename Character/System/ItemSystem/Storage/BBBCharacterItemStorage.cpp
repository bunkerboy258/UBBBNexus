
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Storage/BBBCharacterItemStorage.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"

bool FBBBCharacterItemStorage::GetBackpackItem(const FBBBCharacterItemState &ItemState, int32 BackpackSlot, FBBBItemInstance &OutItem) const
{
    const FBBBCharacterItemInventoryState &Inventory = ItemState.Inventory;

    if (BackpackSlot < 0 || BackpackSlot >= Inventory.MainInventoryCapacity)
    {
        return false;
    }

    for (const FBBBInventoryEntry &Entry : Inventory.MainInventory.Entries)
    {
        if (Entry.SlotIndex == BackpackSlot && Entry.ItemInstance.IsValid())
        {
            OutItem = Entry.ItemInstance;
            return true;
        }
    }
    return false;
}

bool FBBBCharacterItemStorage::GetHotbarItem(const FBBBCharacterItemState &ItemState, int32 HotbarSlot, FBBBItemInstance &OutItem) const
{
    const FBBBCharacterItemInventoryState &Inventory = ItemState.Inventory;

    if (HotbarSlot < 0 || HotbarSlot >= Inventory.HotbarCapacity)
    {
        return false;
    }
    return FindItemInstance(ItemState, Inventory.HotbarItemInstanceIds[HotbarSlot], OutItem);
}

bool FBBBCharacterItemStorage::AddItem(FBBBCharacterItemState &ItemState, const FBBBItemInstance &ItemInstance) const
{

    if (!ItemInstance.IsValid())
    {
        return false;
    }
    const int32 Capacity = ItemState.Inventory.MainInventoryCapacity;
    if (ItemState.Inventory.MainInventory.GetNumOccupiedSlots() >= Capacity)
    {
        return false;
    }

    for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
    {
        int32 ExistingIndex = INDEX_NONE;
        if (!FindEntryIndex(ItemState, SlotIndex, ExistingIndex))
        {
            return AddItemToSlot(ItemState, SlotIndex, ItemInstance);
        }
    }
    return false;
}

bool FBBBCharacterItemStorage::AddItemToSlot(
    FBBBCharacterItemState &ItemState,
    int32 SlotIndex,
    const FBBBItemInstance &ItemInstance) const
{

    if (!ItemInstance.IsValid())
    {
        return false;
    }

    if (SlotIndex < 0 || SlotIndex >= ItemState.Inventory.MainInventoryCapacity)
    {
        return false;
    }
    int32 ExistingIndex = INDEX_NONE;
    if (FindEntryIndex(ItemState, SlotIndex, ExistingIndex))
    {
        return false;
    }

    FBBBInventoryEntry Entry;
    Entry.SlotIndex = SlotIndex;
    Entry.ItemInstance = ItemInstance;
    ItemState.Inventory.MainInventory.Entries.Add(Entry);
    return true;
}

bool FBBBCharacterItemStorage::AssignHotbarItem(
    FBBBCharacterItemState &ItemState,
    int32 HotbarSlot,
    const FGuid &ItemInstanceId) const
{

    if (HotbarSlot < 0 || HotbarSlot >= ItemState.Inventory.HotbarCapacity)
    {
        return false;
    }
    FBBBItemInstance ItemInstance;
    if (!FindItemInstance(ItemState, ItemInstanceId, ItemInstance))
    {
        return false;
    }

    ItemState.Inventory.HotbarItemInstanceIds[HotbarSlot] = ItemInstanceId;
    return true;
}

bool FBBBCharacterItemStorage::FindEntryIndex(const FBBBCharacterItemState &ItemState, int32 SlotIndex, int32 &OutEntryIndex)
{
    const TArray<FBBBInventoryEntry> &Entries = ItemState.Inventory.MainInventory.Entries;
    for (int32 Index = 0; Index < Entries.Num(); ++Index)
    {
        if (Entries[Index].SlotIndex == SlotIndex)
        {
            OutEntryIndex = Index;
            return true;
        }
    }
    return false;
}

bool FBBBCharacterItemStorage::FindItemInstance(
    const FBBBCharacterItemState &ItemState,
    const FGuid &ItemInstanceId,
    FBBBItemInstance &OutItem)
{
    if (!ItemInstanceId.IsValid())
    {
        return false;
    }
    for (const FBBBInventoryEntry &Entry : ItemState.Inventory.MainInventory.Entries)
    {
        if (Entry.ItemInstance.InstanceId == ItemInstanceId)
        {
            OutItem = Entry.ItemInstance;
            return true;
        }
    }
    return false;
}
