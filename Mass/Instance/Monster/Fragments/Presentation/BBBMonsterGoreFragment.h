#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterGoreWound.h"
#include "BBBMonsterGoreFragment.generated.h"

/** 唯一当前伤口集合 前五处持久破损 后两处普通创口 全部由同一内核呈现 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterGoreFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 固定容量不随射击增长 不另存断肢位域 */
    FBBBMonsterGoreWound Wounds[7];

    /** 已消费当前命中序号 */
    uint32 ConsumedSerial = 0;

    /** 普通伤口更新预算 不影响程序化受击 */
    float NextWoundTime = 0;

    /** 普通伤口轮换位置 不覆盖永久破损 */
    uint8 NextSurfaceSlot = 0;
};
