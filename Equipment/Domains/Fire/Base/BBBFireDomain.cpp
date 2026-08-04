#include "BBBWork/UBBBNexus/Equipment/Domains/Fire/Base/BBBFireDomain.h"

#include "BBBWork/UBBBNexus/Equipment/Domains/Fire/Definition/BBBFireRuntimeData.h"

UBBBFireRuntimeData *UBBBFireDomain::InitializeRuntimeData(UObject &Outer) const
{
    return NewObject<UBBBFireRuntimeData>(&Outer);
}
