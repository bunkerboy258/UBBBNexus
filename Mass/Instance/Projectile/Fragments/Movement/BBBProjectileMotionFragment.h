#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBProjectileMotionFragment.generated.h"

/** 子弹运动数据 */
USTRUCT()
struct ABBB_EVAC_API FBBBProjectileMotionFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 子弹出生时的枪口位置 */
    FVector SpawnLocation = FVector::ZeroVector;

    /** 当前步进前的位置 */
    FVector PreviousLocation = FVector::ZeroVector;

    /** 是否已经消费出生输入 */
    bool bInitialized = false;

    /** 当前弹丸使用的世界重力倍率 */
    float GravityScale = 0.0f;

    /** 碰撞后停止运动 引信仍继续计时 */
    bool bResting = false;
};
