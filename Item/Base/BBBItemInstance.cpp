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

void UBBBItemInstance::Configure(UBBBItemDefinition &InDefinition, int32 InStackCount)
{
    InstanceId = FGuid::NewGuid();
    Definition = &InDefinition;
    StackCount = FMath::Clamp(InStackCount, 1, InDefinition.MaxStack);
}
