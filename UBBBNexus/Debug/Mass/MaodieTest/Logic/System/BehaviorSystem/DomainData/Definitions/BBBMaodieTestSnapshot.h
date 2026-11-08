#pragma once
#include "CoreMinimal.h"
#include "BBBMaodieTestSnapshot.generated.h"
/** 当前表现事实 Phase 为零待机 一移动 二观察 三哈气 */
USTRUCT()
struct FBBBMaodieTestSnapshot final
{
    GENERATED_BODY()
    /** 当前动作 */
    UPROPERTY()
    uint8 Phase = 0;
    /** 当前是否注视目标 */
    UPROPERTY()
    bool bHasLookTarget = false;
    /** 当前注视的世界位置 */
    UPROPERTY()
    FVector LookTarget = FVector::ZeroVector;
    /** 哈气触发序号 */
    UPROPERTY()
    uint32 HissSerial = 0;
};
