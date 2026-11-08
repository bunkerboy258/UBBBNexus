#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "BBBMonsterTargetFragment.generated.h"

/** 小怪当前追踪目标请求 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterTargetFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 最近一次确认的目标坐标 */
    FVector TargetLocation = FVector::ZeroVector;

    /** 最近一次侦察到目标的时间 */
    float LastSeenTime = -FLT_MAX;

    /** 当前是否持有有效目标 */
    bool bHasTarget = false;

    /** 感知确认的实际目标对象 */
    TWeakObjectPtr<AActor> TargetActor;

    /** 当前目标由本机前方视觉确认 此值为假时不能启动新攻击 */
    bool bTargetVisible = false;

    /** 追踪位置或目标身份发生改变时递增 */
    uint32 Revision = 0;

    /** 当前线索成立的时间 */
    float EvidenceTime = -FLT_MAX;
};
