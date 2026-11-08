#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterStimulusFragment.generated.h"

/** 外部投递的当前声音线索 不保存声音事件队列 */
USTRUCT()
struct FBBBMonsterStimulusFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 本次声音发生的位置 */
    FVector Position = FVector::ZeroVector;

    /** 本次声音成立的世界时间 */
    float Time = -FLT_MAX;

    /** 当前声音线索的有效截止时间 */
    float EndsAt = -FLT_MAX;
};
