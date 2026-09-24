#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

TSubclassOf<ABBBEquipment> UBBBEquipmentCatalog::FindEquipmentClass(FName EquipmentHandle) const
{
    // 空句柄不能解析装备定义
    if (EquipmentHandle.IsNone())
    {
        return nullptr;
    }

    TSubclassOf<ABBBEquipment> MatchingClass;
    for (const TSubclassOf<ABBBEquipment> &EquipmentClass : EquipmentClasses)
    {
        if (!EquipmentClass)
        {
            ensureMsgf(false, TEXT("装备目录包含空 Actor 类"));
            continue;
        }

        const ABBBEquipment *ClassDefault = EquipmentClass.GetDefaultObject();
        if (!ensureMsgf(
            ClassDefault && ClassDefault->GetDefinition() && !ClassDefault->GetEquipmentId().IsNone(),
            TEXT("装备类 %s 缺少有效的装备配置或标识"),
            *EquipmentClass->GetName()))
        {
            continue;
        }

        if (ClassDefault->GetEquipmentId() != EquipmentHandle)
        {
            continue;
        }

        if (!ensureMsgf(
            !MatchingClass,
            TEXT("装备目录存在重复标识 %s"),
            *EquipmentHandle.ToString()))
        {
            return nullptr;
        }

        MatchingClass = EquipmentClass;
    }

    return MatchingClass;
}
