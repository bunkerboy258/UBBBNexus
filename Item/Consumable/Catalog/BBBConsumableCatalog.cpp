#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"

const FBBBConsumableCatalogEntry *UBBBConsumableCatalog::FindItem(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }
    const FBBBConsumableCatalogEntry *Found = nullptr;
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
    if (!Found || Found->Definition->ItemType != EBBBItemType::Consumable
        || !Found->Definition->WearSlot.IsNone() || Found->Definition->MaxStack < 1 || !Found->ActorClass || Found->ActorClass.GetDefaultObject()->GetDefinition() != Found->Definition.Get())
    {
        return nullptr;
    }
    return Found;
}
