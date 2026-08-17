#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"

UBBBEquipmentDefinition *UBBBEquipmentCatalog::FindDefinition(FName EquipmentHandle) const
{
    if (EquipmentHandle.IsNone())
    {
        return nullptr;
    }

    for (UBBBEquipmentDefinition *Definition : Definitions)
    {
        if (Definition && Definition->EquipmentId == EquipmentHandle)
        {
            return Definition;
        }
    }

    return nullptr;
}
