
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/BBBFireFragment.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/BBBFireRuntimeData.h"

UBBBItemFragmentRuntimeData *UBBBFireFragment::InitializeRuntimeData(UObject &Outer) const
{

    return NewObject<UBBBFireRuntimeData>(&Outer);
}
