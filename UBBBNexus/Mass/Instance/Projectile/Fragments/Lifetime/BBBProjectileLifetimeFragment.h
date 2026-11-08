#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBProjectileLifetimeFragment.generated.h"

/** 子弹寿命数据 */
USTRUCT()
struct ABBB_EVAC_API FBBBProjectileLifetimeFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 剩余秒数 */
    float RemainingSeconds = 0.0f;

    /** 碰撞或寿命结束后请求回收 */
    bool bPendingDestroy = false;

    /** 剩余引信时间 零表示没有延时引爆 */
    float FuseRemainingSeconds = 0.0f;
};
