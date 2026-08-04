#include "BBBWork/UBBBNexus/Equipment/Definition/BBBEquipmentRuntimeData.h"

#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Equip/BBBEquipDomain.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Fire/Base/BBBFireDomain.h"
#include "BBBWork/UBBBNexus/Equipment/Domains/Magazine/BBBMagazineDomain.h"

void UBBBEquipmentRuntimeData::Initialize(const UBBBEquipmentDefinition &Definition)
{
    Equip = Definition.EquipDomain
        ? Definition.EquipDomain->InitializeRuntimeData(*this)
        : nullptr;

    Fire = Definition.FireDomain
        ? Definition.FireDomain->InitializeRuntimeData(*this)
        : nullptr;

    Magazine = Definition.MagazineDomain
        ? Definition.MagazineDomain->InitializeRuntimeData(*this)
        : nullptr;
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
