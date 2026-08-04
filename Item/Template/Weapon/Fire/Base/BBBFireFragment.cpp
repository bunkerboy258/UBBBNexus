
#include "BBBWork/UBBBNexus/Item/Template/Weapon/Fire/Base/BBBFireFragment.h"
#include "BBBWork/UBBBNexus/Item/Template/Weapon/Fire/BBBFireRuntimeData.h"

UBBBItemFragmentRuntimeData *UBBBFireFragment::InitializeRuntimeData(UObject &Outer) const
{

    return NewObject<UBBBFireRuntimeData>(&Outer);
}
