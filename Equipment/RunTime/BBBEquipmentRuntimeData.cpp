#include "BBBWork/UBBBNexus/Equipment/RunTime/BBBEquipmentRuntimeData.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineDomin.h"

void UBBBEquipmentRuntimeData::Initialize(const UBBBEquipmentDefinition &Definition)
{
    if (Definition.EquipDomin.IsValid())
    {
        Equip = Definition.EquipDomin.Get().InitializeRuntimeData(*this);
    }

    if (Definition.FireDomin.IsValid())
    {
        Fire = Definition.FireDomin.Get().InitializeRuntimeData(*this);
    }

    if (Definition.MagazineDomin.IsValid())
    {
        Magazine = Definition.MagazineDomin.Get().InitializeRuntimeData(*this);
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
