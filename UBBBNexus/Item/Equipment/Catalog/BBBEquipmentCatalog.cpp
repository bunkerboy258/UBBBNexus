#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"

TSubclassOf<ABBBEquipment> UBBBEquipmentCatalog::FindItem(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }
    TSubclassOf<ABBBEquipment> Found;
    for (const auto &ActorClass : Items)
    {
        const auto *Actor = ActorClass ? ActorClass.GetDefaultObject() : nullptr;
        const auto *Definition = Actor ? Actor->GetDefinition() : nullptr;
        if (!IsValid(Definition) || Definition->ItemId != ItemId)
        {
            continue;
        }
        if (Found || Definition->ItemType != EBBBItemType::Equipment
            || !Definition->WearSlot.IsNone() || Definition->MaxStack != 1)
        {
            return nullptr;
        }
        Found = ActorClass;
    }
    return Found;
}
