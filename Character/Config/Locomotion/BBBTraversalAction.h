#pragma once

#include "CoreMinimal.h"
#include "BBBTraversalAction.generated.h"

/** 翻越判断产生的动作事实 不包含动画资产引用 */
UENUM(BlueprintType)
enum class EBBBTraversalAction : uint8
{
    /** 当前没有翻越动作 */
    None,
    /** 越过低矮且较薄的障碍 */
    Vault,
    /** 登上较低的平台 */
    ClimbLow,
    /** 登上较高的平台 */
    ClimbHigh
};
