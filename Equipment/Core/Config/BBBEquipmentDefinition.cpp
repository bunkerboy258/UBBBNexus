#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"

#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/Fragment/BBBStandardEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Fragment/BBBSingleProjectileFireFragment.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Fragment/BBBMagazineReloadFragment.h"

UBBBEquipmentDefinition::UBBBEquipmentDefinition()
{
    EquipFragment.InitializeAs<FBBBStandardEquipFragment>();
    FireFragment.InitializeAs<FBBBSingleProjectileFireFragment>();
    ReloadFragment.InitializeAs<FBBBMagazineReloadFragment>();
}
