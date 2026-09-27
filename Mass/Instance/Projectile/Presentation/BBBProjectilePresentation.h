#pragma once

#include "CoreMinimal.h"

class UWorld;
struct FTransformFragment;
struct FBBBProjectileMotionFragment;
struct FBBBProjectilePresentationFragment;

/** 批量光效数据通道桥接 不持有子弹玩法状态 */
struct FBBBProjectilePresentation final
{
    /**
     * @param World	当前世界
     * @param Transforms	当前位置
     * @param Motion	本帧起点
     * @param Presentation	表现事实
     * @return 无
     */
    static void Publish(UWorld& World, TConstArrayView<FTransformFragment> Transforms,
        TConstArrayView<FBBBProjectileMotionFragment> Motion,
        TConstArrayView<FBBBProjectilePresentationFragment> Presentation);
};
