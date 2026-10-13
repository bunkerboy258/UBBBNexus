#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"

const FBBBEquipmentCatalogEntry *UBBBEquipmentCatalog::FindItem(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }
    const FBBBEquipmentCatalogEntry *Found = nullptr;
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
    if (!Found || Found->Definition->ItemType != EBBBItemType::Equipment
        || !Found->Definition->WearSlot.IsNone() || Found->Definition->MaxStack < 1 || Found->Definition->MaxStack != 1 || !Found->ActorClass || Found->ActorClass.GetDefaultObject()->GetDefinition() != Found->Definition.Get())
    {
        return nullptr;
    }
    return Found;
}
