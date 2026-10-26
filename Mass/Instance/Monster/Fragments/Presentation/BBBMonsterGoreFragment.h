#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterGoreFragment.generated.h"

/** 当前二级破坏结果 毁伤八区与五个断肢身份各自持久保存 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterGoreFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 顺序对应头 左臂 右臂 左腿 右腿 */
    uint8 DismembermentPresets[5] = {};

    /** 由全部玩家的当前贡献推导 255 表示完全毁伤 */
    uint8 DestructionStrengths[8] = {};
};
