#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBNetworkStates.generated.h"

/** 需要从控制端同步到模拟端的移动状态 */
USTRUCT()
struct FBBBLocomotionNetworkState
{
    GENERATED_BODY()

    UPROPERTY()
    EBBBCharacterGait Gait = EBBBCharacterGait::Run;
};

USTRUCT()
struct FBBBAimNetworkState
{
    GENERATED_BODY()

    UPROPERTY()
    bool bIsAiming = false;

    UPROPERTY()
    FVector_NetQuantize10 AimTargetWorld = FVector::ZeroVector;
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

/** 移动状态上传器的跨帧观测值 */
USTRUCT()
struct FBBBLocomotionNetworkObserverState
{
    GENERATED_BODY()

    TOptional<FBBBLocomotionNetworkState> LastObservedState;
};
