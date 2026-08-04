#include "BBBWork/UBBBNexus/Item/CatalogSystem/BBBEquipmentCatalog.h"

#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"

UBBBItemDefinition *UBBBEquipmentCatalog::FindDefinition(FName EquipmentHandle)
{
    if (EquipmentHandle.IsNone())
    {
        return nullptr;
    }

    for (UBBBItemDefinition *Definition : Definitions)
    {
        if (Definition && Definition->ItemId == EquipmentHandle)
        {
            return Definition;
        }
    }

    return nullptr;
}
