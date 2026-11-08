#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterGait.generated.h"

/** 追击目标速度档位 不作为动画状态 */
UENUM(BlueprintType)
enum class EBBBMonsterGait : uint8
{
    /** 缓慢接近或巡逻 */
    Walk UMETA(DisplayName = "走"),
    /** 中距离追击 */
    Run UMETA(DisplayName = "跑"),
    /** 远距离追赶 */
    Sprint UMETA(DisplayName = "冲刺")
};
