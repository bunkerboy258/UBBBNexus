#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Config/BBBEquipmentDefinition.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/Fragment/BBBStandardEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/Fragment/BBBSingleProjectileFireFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Fragment/BBBMagazineReloadFragment.h"

UBBBEquipmentDefinition::UBBBEquipmentDefinition()
{
    // 为装备提供默认的装备开火和换弹实现
    EquipFragment.InitializeAs<FBBBStandardEquipFragment>();
    FireFragment.InitializeAs<FBBBSingleProjectileFireFragment>();
    ReloadFragment.InitializeAs<FBBBMagazineReloadFragment>();
}
