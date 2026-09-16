#include "BBBWork/UBBBNexus/Equipment/RunTime/BBBEquipmentRuntimeData.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Reload/Definition/BBBReloadRuntimeData.h"

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

    if (Definition.ReloadDomain.IsValid())
    {
        Reload = NewObject<UBBBReloadRuntimeData>(this);
    }

    if (Equip && Definition.FireDomin.IsValid())
    {
        Equip->MuzzleSocketName = Definition.FireDomin.Get().GetMuzzleSocketName();
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

UBBBReloadRuntimeData *UBBBEquipmentRuntimeData::GetReload() const
{
    return Reload;
}

