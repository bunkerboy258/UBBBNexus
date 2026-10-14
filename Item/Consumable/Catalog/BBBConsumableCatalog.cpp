#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"

TSubclassOf<ABBBConsumable> UBBBConsumableCatalog::FindItem(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }
    TSubclassOf<ABBBConsumable> Found;
    for (const auto &ActorClass : Items)
    {
        const auto *Actor = ActorClass ? ActorClass.GetDefaultObject() : nullptr;
        const auto *Definition = Actor ? Actor->GetDefinition() : nullptr;
        if (!IsValid(Definition) || Definition->ItemId != ItemId)
        {
            continue;
        }
        if (Found || Definition->ItemType != EBBBItemType::Consumable
            || !Definition->WearSlot.IsNone() || Definition->MaxStack < 1)
        {
            return nullptr;
        }
        Found = ActorClass;
    }
    return Found;
}
