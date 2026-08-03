#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"

#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"

const FGuid &UBBBItemInstance::GetInstanceId() const
{
    return InstanceId;
}

UBBBItemDefinition *UBBBItemInstance::GetDefinition() const
{
    return Definition;
}

UBBBItemRuntimeData *UBBBItemInstance::GetRuntimeData() const
{
    return RuntimeData;
}

int32 UBBBItemInstance::GetStackCount() const
{
    return StackCount;
}

bool UBBBItemInstance::IsValid() const
{
    return InstanceId.IsValid() && Definition != nullptr;
}
