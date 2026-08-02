
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/BBBItemTypes.h"
#include "BBBItemInstance.generated.h"
class UBBBItemDefinition;
class UBBBItemRuntimeData;


USTRUCT(BlueprintType)
struct FBBBItemInstance
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGuid InstanceId;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBBBItemRuntimeData> RuntimeData = nullptr;

    UPROPERTY(BlueprintReadOnly)
    int32 StackCount = 1;

    bool IsValid() const
    {
        return InstanceId.IsValid() && Definition != nullptr;
    }
};
