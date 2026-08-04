#include "BBBWork/UBBBNexus/Item/Equipment/Fire/Base/BBBFireOperation.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Fire/BBBFireRuntimeData.h"

UBBBFireRuntimeData *UBBBFireOperation::InitializeRuntimeData(UObject &Outer) const
{
    return NewObject<UBBBFireRuntimeData>(&Outer);
}
