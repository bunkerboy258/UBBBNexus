#include "BBBWork/UBBBNexus/Item/Wearable/Catalog/BBBWearableCatalog.h"

const FBBBWearableCatalogEntry *UBBBWearableCatalog::FindItem(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }
    const FBBBWearableCatalogEntry *Found = nullptr;
    for (const auto &Entry : Items)
    {
        if (!IsValid(Entry.Definition) || Entry.Definition->ItemId != ItemId)
        {
            continue;
        }
        if (Found)
        {
            return nullptr;
        }
        Found = &Entry;
    }
    if (!Found || Found->Definition->ItemType != EBBBItemType::Wearable
        || Found->Definition->WearSlot.IsNone() || Found->Definition->MaxStack < 1 || Found->Definition->MaxStack != 1)
    {
        return nullptr;
    }
    return Found;
}
