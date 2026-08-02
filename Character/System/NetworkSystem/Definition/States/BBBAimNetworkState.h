#pragma once
#include "CoreMinimal.h"
#include "BBBAimNetworkState.generated.h"

USTRUCT()
struct FBBBAimNetworkState
{
    GENERATED_BODY()

    UPROPERTY()
    bool bIsAiming = false;

    UPROPERTY()
    FVector_NetQuantize10 AimIKTargetWorld = FVector::ZeroVector;
    /*UE网络压缩向量类型 每个分量只保留到小数点后 1 位*/
};
