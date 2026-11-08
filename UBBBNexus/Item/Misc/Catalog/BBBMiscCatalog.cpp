#include "BBBWork/UBBBNexus/Item/Misc/Catalog/BBBMiscCatalog.h"

const UBBBItemDefinition *UBBBMiscCatalog::FindItem(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }
    const UBBBItemDefinition *Found = nullptr;
    for (const UBBBItemDefinition *Definition : Items)
    {
        if (!IsValid(Definition) || Definition->ItemId != ItemId)
        {
            continue;
        }
        if (Found)
        {
            return nullptr;
        }
        Found = Definition;
    }
    return Found && Found->ItemType == EBBBItemType::Misc && Found->WearSlot.IsNone()
        && Found->MaxStack > 0 ? Found : nullptr;
}
