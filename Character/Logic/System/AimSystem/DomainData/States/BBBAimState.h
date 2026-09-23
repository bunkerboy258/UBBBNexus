
#pragma once
#include "CoreMinimal.h"
#include "BBBAimState.generated.h"

USTRUCT(BlueprintType)
//角色运行时瞄准状态数据
struct FBBBAimState final
{
    GENERATED_BODY()

    //是否保持瞄准姿态
    UPROPERTY(BlueprintReadOnly)
    bool bIsAiming = false;

    /** 角色瞄准动画权重 */
    UPROPERTY(BlueprintReadOnly)
    float AimAlpha = 0.0f;

    //理想瞄准目标点的世界坐标
    UPROPERTY(BlueprintReadOnly)
    FVector AimTargetWorld = FVector::ZeroVector;

};
