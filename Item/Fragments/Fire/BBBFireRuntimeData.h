
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/RuntimeData/BBBItemFragmentRuntimeData.h"
#include "BBBFireRuntimeData.generated.h"

UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBFireRuntimeData final : public UBBBItemFragmentRuntimeData
{
    GENERATED_BODY()
public:

    UPROPERTY(BlueprintReadOnly)
    float LastFireTime = -1000.0f;
};
