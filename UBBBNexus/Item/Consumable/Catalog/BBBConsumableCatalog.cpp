#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
UBBBConsumableDefinition *UBBBConsumableCatalog::FindItem(FName ItemId) const
{
    UBBBConsumableDefinition *Found = nullptr;
    if (ItemId.IsNone())
    {
        return nullptr;
    }
    for (const auto &Definition : Items)
    {
        if (!IsValid(Definition) || Definition->ItemId != ItemId)
        {
            continue;
        }
        if (Found || !Definition->RuntimeClass || Definition->ItemType != EBBBItemType::Consumable ||
            !Definition->WearSlot.IsNone() || Definition->MaxStack < 1)
        {
            return nullptr;
        }
        Found = Definition;
    }
    return Found;
}
