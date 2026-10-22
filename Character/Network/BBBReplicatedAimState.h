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

    /** 同载体同步的角色开火表现意图 不表示发射事实 */
    UPROPERTY()
    bool bFire = false;

    /** 使用网络定点压缩的世界空间瞄准目标 */
    UPROPERTY()
    FVector_NetQuantize10 AimTargetWorld = FVector::ZeroVector;
};
