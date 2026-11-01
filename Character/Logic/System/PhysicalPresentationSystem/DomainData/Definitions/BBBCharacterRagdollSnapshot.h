#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterRagdollSnapshot.generated.h"

/** 已进入布娃娃的当前骨盆事实 不保存逐骨骼历史 */
USTRUCT()
struct FBBBCharacterRagdollSnapshot final
{
    GENERATED_BODY()
    /** 当前实例内递增的物理结果版本 零表示尚未进入 */
    UPROPERTY()
    uint64 Revision = 0;
    /** 物理骨盆的世界位置 */
    UPROPERTY()
    FVector PelvisLocation = FVector::ZeroVector;
    /** 物理骨盆的世界线速度 */
    UPROPERTY()
    FVector Velocity = FVector::ZeroVector;
};
