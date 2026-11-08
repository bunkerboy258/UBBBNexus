#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterHitRegion.generated.h"

/** 命中表现部位 不改变伤害倍率 */
UENUM(BlueprintType)
enum class EBBBMonsterHitRegion : uint8
{
    /** 躯干 */
    Torso UMETA(DisplayName = "躯干"),
    /** 头部 */
    Head UMETA(DisplayName = "头部"),
    /** 左臂 */
    LeftArm UMETA(DisplayName = "左臂"),
    /** 右臂 */
    RightArm UMETA(DisplayName = "右臂"),
    /** 左腿 */
    LeftLeg UMETA(DisplayName = "左腿"),
    /** 右腿 */
    RightLeg UMETA(DisplayName = "右腿")
};
