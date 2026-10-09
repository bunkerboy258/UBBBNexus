#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterVariationFragment.generated.h"

/** 一代实体的固定出生特征 不持有资产或历史队列 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterVariationFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 只由稳定出生身份派生的正数种子 */
    uint32 Seed = 0;

    /** 循环动作归一化起点 */
    float PhaseOffset = 0.0f;

    /** 实际速度相对基础配置的固定比例 */
    float SpeedScale = 1.0f;

    /** 零到二百五十五的感染度 */
    uint8 Infection = 0;

    /** 三种稳定移动风格之一 */
    uint8 LocomotionStyle = 0;
};
