#pragma once

#include "CoreMinimal.h"

struct FBBBMonsterNavigationFragment;

/** Detour 走廊与 Mass 实际移动之间的机械交接 */
struct FBBBMonsterNavigation
{
    /**
     * @param Path	当前路线
     * @return 无
     */
    static void Reset(FBBBMonsterNavigationFragment& Path);
    /**
     * @param Path	当前路线
     * @param Feet	实际脚底位置
     * @return 无
     */
    static void Feedback(FBBBMonsterNavigationFragment& Path, const FVector& Feet);
    /**
     * @param Path	当前路线
     * @param Feet	期望脚底位置
     * @return 导航网格允许的脚底位置
     */
    static FVector Constrain(const FBBBMonsterNavigationFragment& Path, const FVector& Feet);
    /**
     * @param Path	当前路线
     * @param Feet	跨障后的实际脚底
     * @return 无
     */
    static void FinishTraversal(FBBBMonsterNavigationFragment& Path, const FVector& Feet);
};
