#include "BBBWork/UBBBNexus/Item/Definition/BBBItemRuntimeData.h"

#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Fire/Base/BBBFireOperation.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Magazine/BBBMagazineOperation.h"

void UBBBItemRuntimeData::Initialize(const UBBBItemDefinition &Definition)
{
    Fire = Definition.FireOperation
        ? Definition.FireOperation->InitializeRuntimeData(*this)
        : nullptr;

    Magazine = Definition.MagazineOperation
        ? Definition.MagazineOperation->InitializeRuntimeData(*this)
        : nullptr;
}

UBBBFireRuntimeData *UBBBItemRuntimeData::GetFire() const
{
    return Fire;
}

UBBBMagazineRuntimeData *UBBBItemRuntimeData::GetMagazine() const
{
    return Magazine;
}
