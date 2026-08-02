
#pragma once
#include "CoreMinimal.h"
#include "BBBAimRuntimeTypes.generated.h"

USTRUCT(BlueprintType)
//保存一次相机视线检测的完整结果
struct FBBBAimTraceResult
{
    GENERATED_BODY()

    //射线构建过程是否成功
    UPROPERTY(BlueprintReadOnly)
    bool bValid = false;

    //相机视线检测起点
    UPROPERTY(BlueprintReadOnly)
    FVector_NetQuantize ViewStart = FVector::ZeroVector;

    //命中时的最远检测终点
    UPROPERTY(BlueprintReadOnly)
    FVector_NetQuantize ViewEnd = FVector::ZeroVector;

    //命中点或最远检测点作为最终目标
    UPROPERTY(BlueprintReadOnly)
    FVector_NetQuantize AimTarget = FVector::ZeroVector;

    //由相机旋转生成的单位视线方向
    UPROPERTY(BlueprintReadOnly)
    FVector ViewDirection = FVector::ForwardVector;
};

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
