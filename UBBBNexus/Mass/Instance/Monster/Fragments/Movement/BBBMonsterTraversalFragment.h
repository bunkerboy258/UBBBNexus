#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterTraversalFact.h"
#include "BBBMonsterTraversalFragment.generated.h"

class ABBBMonsterClimbLinkActor;

/** 攀爬处理器维护的当前通行与重试状态 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterTraversalFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前可同步的动作结果 */
    FBBBMonsterTraversalFact Fact;

    /** 当前占用的台沿入口 */
    TWeakObjectPtr<ABBBMonsterClimbLinkActor> Link;

    /** 允许再次检查入口的世界时刻 */
    float RetryAt = 0.0f;

    /** 最近一次拒绝或中断原因 */
    FName Failure;
};
