#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"

const FBBBItemCatalogEntry *UBBBItemCatalog::FindItem(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }
    const FBBBItemCatalogEntry *Found = nullptr;
    for (const FBBBItemCatalogEntry &Entry : Items)
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
    if (!Found)
    {
        return nullptr;
    }
    if (Found->Definition->ItemType == EBBBItemType::Equipment)
    {
        const ABBBEquipment *Default = Found->EquipmentClass.GetDefaultObject();
        if (!Default || Default->GetDefinition() != Found->Definition.Get()
            || !Found->Definition->WearSlot.IsNone())
        {
            return nullptr;
        }
        return Found;
    }
    if ((Found->Definition->ItemType != EBBBItemType::Wearable && Found->Definition->ItemType != EBBBItemType::Misc)
        || Found->EquipmentClass || Found->Definition->WearSlot.IsNone())
    {
        return nullptr;
    }
    return Found;
}

TSubclassOf<ABBBEquipment> UBBBItemCatalog::FindEquipmentClass(const FName ItemId) const
{
    const FBBBItemCatalogEntry *Entry = FindItem(ItemId);
    return Entry ? Entry->EquipmentClass : nullptr;
}
