#pragma once
#include "CoreMinimal.h"
#include "BBBAimResults.generated.h"

USTRUCT(BlueprintType)
//保存一次相机视线检测结果
struct FBBBAimTraceResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bValid = false;

    UPROPERTY(BlueprintReadOnly)
    FVector_NetQuantize AimTarget = FVector::ZeroVector;
};
