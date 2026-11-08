#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterBehavior.generated.h"

/** 小怪当前的权威逻辑状态 */
UENUM(BlueprintType)
enum class EBBBMonsterBehavior : uint8
{
    /** 未发现目标时的待机状态 */
    Idle,

    /** 等待玩家进入可侦察范围 */
    Alert UMETA(DisplayName = "警觉"),

    /** 随机方向的限时步行 */
    Patrol UMETA(DisplayName = "巡逻"),

    /** 沿导航路径接近目标 */
    Chase,

    /** 处于攻击距离内 */
    Attack,

    /** 生命值归零后的死亡状态 */
    Dead
};
