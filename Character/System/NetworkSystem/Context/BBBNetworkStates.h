#pragma once
#include "CoreMinimal.h"
#include "BBBNetworkStates.generated.h"

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

USTRUCT()
//保存瞄准状态上传器的跨帧观测值
struct FBBBAimNetworkObserverState
{
    GENERATED_BODY()

    TOptional<FBBBAimNetworkState> LastObservedState;

    float LastUploadTime = -1000.0f;
};
