#pragma once
#include "BBBMonsterSpawnRegionRole.generated.h"

/** 地图生成区域的业务用途 */
UENUM(BlueprintType)
enum class EBBBMonsterSpawnRegionRole : uint8
{
    /** 固有僵尸刷怪区 玩家靠近时一次性铺怪 */
    Population UMETA(DisplayName = "固有刷怪区"),
    /** 预制波次刷怪区 供事件波次按标识引用 */
    Event UMETA(DisplayName = "预制波次区"),
    /** 禁刷区 例如安全屋 禁止任何刷怪落点 */
    BlockSpawn UMETA(DisplayName = "禁刷区")
};
