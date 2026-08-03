
#pragma once
#include "CoreMinimal.h"
#include "BBBAimStates.generated.h"

USTRUCT(BlueprintType)
//角色运行时瞄准状态数据
struct FBBBAimRuntimeState
{
    GENERATED_BODY()

    //是否保持瞄准姿态
    UPROPERTY(BlueprintReadOnly)
    bool bIsAiming = false;

    //理想瞄准目标点的世界坐标
    UPROPERTY(BlueprintReadOnly)
    FVector_NetQuantize AimTargetWorld = FVector::ZeroVector;

    //动画系统的目标点的世界坐标
    UPROPERTY(BlueprintReadOnly)
    FVector_NetQuantize10 AimIKTargetWorld = FVector::ZeroVector;

    //瞄准射线起点
    UPROPERTY(BlueprintReadOnly)
    FVector_NetQuantize AimTraceStart = FVector::ZeroVector;

    //瞄准射线终点
    UPROPERTY(BlueprintReadOnly)
    FVector_NetQuantize AimTraceEnd = FVector::ZeroVector;

    //相对角色前轴的水平瞄准角
    UPROPERTY(BlueprintReadOnly)
    float AimYaw = 0.0f;
};

//保存远端同步后交给瞄准领域的状态
struct FBBBRemoteAimState
{
    bool bIsAiming = false;

    FVector AimTargetWorld = FVector::ZeroVector;
};
