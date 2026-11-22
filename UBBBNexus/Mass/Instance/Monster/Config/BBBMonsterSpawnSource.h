#pragma once
#include "BBBMonsterSpawnSource.generated.h"

/** 小怪出生的调度来源分类 */
UENUM(BlueprintType)
enum class EBBBMonsterSpawnSource : uint8
{
    /** 独立生成器 */
    Spawner UMETA(DisplayName = "独立生成器"),
    /** 导演固有区域 */
    Population UMETA(DisplayName = "固有僵尸"),
    /** 导演动态波次 */
    DynamicHorde UMETA(DisplayName = "动态波次"),
    /** 导演预制事件波次 */
    EventWave UMETA(DisplayName = "预制波次")
};
