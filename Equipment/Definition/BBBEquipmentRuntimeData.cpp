#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentRuntimeData.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBSingleProjectileFireFragment.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineFragment.h"

void UBBBEquipmentRuntimeData::Initialize(const UBBBEquipmentDefinition &Definition)
{
    Equip = Definition.EquipFragment.InitializeRuntimeData(*this);

    if (Definition.FireFragment.bEnabled)
    {
        Fire = Definition.FireFragment.InitializeRuntimeData(*this);
    }

    if (Definition.MagazineFragment.bEnabled)
    {
        Magazine = Definition.MagazineFragment.InitializeRuntimeData(*this);
    }
}

UBBBEquipRuntimeData *UBBBEquipmentRuntimeData::GetEquip() const
{
    return Equip;
}

UBBBFireRuntimeData *UBBBEquipmentRuntimeData::GetFire() const
{
    return Fire;
}

UBBBMagazineRuntimeData *UBBBEquipmentRuntimeData::GetMagazine() const
{
    return Magazine;
}
