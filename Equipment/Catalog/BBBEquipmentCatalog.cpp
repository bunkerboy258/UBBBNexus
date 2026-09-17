#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Config/BBBEquipmentDefinition.h"

UBBBEquipmentDefinition *UBBBEquipmentCatalog::FindDefinition(FName EquipmentHandle) const
{
    // 空句柄不能解析装备定义
    if (EquipmentHandle.IsNone())
    {
        return nullptr;
    }

    // 在配置目录中查找匹配的装备定义
    for (UBBBEquipmentDefinition *Definition : Definitions)
    {
        if (Definition && Definition->EquipmentId == EquipmentHandle)
        {
            return Definition;
        }
    }

    // 未找到匹配定义时返回空结果
    return nullptr;
}
