#pragma once
#include "CoreMinimal.h"
#include "Engine/HitResult.h"

/** 本轮身体碰撞的引擎事实 */
struct FBBBCharacterFallContact final
{
    /** 布娃娃轮次 */
    uint64 Episode = 0;
    /** 实际阻挡接触 */
    FHitResult Hit;
    /** 物理求解前该身体部位的向下速度 */
    float DownSpeed = 0;
};
