#include "BBBWork/UBBBNexus/Item/CatalogSystem/BBBEquipmentCatalog.h"

#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"

UBBBEquipmentDefinition *UBBBEquipmentCatalog::FindDefinition(FName EquipmentHandle)
{
    if (EquipmentHandle.IsNone())
    {
        return nullptr;
    }

    for (UBBBEquipmentDefinition *Definition : Definitions)
    {
        if (Definition && Definition->ItemId == EquipmentHandle)
        {
            return Definition;
        }
    }

    return nullptr;
}
