#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"

#include "BBBWork/UBBBNexus/Equipment/Fragment/Equip/BBBStandardEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Fire/BBBSingleProjectileFireFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Reload/BBBMagazineReloadFragment.h"

UBBBEquipmentDefinition::UBBBEquipmentDefinition()
{
    EquipFragment.InitializeAs<FBBBStandardEquipFragment>();
    FireFragment.InitializeAs<FBBBSingleProjectileFireFragment>();
    ReloadFragment.InitializeAs<FBBBMagazineReloadFragment>();
}
