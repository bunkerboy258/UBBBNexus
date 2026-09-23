#pragma once

#include "CoreMinimal.h"
#include "BBBReplicatedAimState.generated.h"

/** 网络原子传输的瞄准最终状态 */
USTRUCT()
struct FBBBReplicatedAimState final
{
    GENERATED_BODY()

    /** 是否保持瞄准 */
    UPROPERTY()
    bool bIsAiming = false;

    /** 角色瞄准动画权重 */
    UPROPERTY()
    float AimAlpha = 0.0f;

    /** 使用网络定点压缩的世界空间瞄准目标 */
    UPROPERTY()
    FVector_NetQuantize10 AimTargetWorld = FVector::ZeroVector;
};
