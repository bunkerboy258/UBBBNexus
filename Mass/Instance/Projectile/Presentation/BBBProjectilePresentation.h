#pragma once

#include "CoreMinimal.h"

class UWorld;
class UNiagaraDataChannelAsset;
struct FBBBProjectileImpact;
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

    /**
     * @param World	当前世界
     * @param Channel	空间命中通道
     * @param Impacts	本次碰撞的表面事实 调用后不保留
     * @return 无
     */
    static void PublishImpacts(UWorld& World, UNiagaraDataChannelAsset& Channel,
        TConstArrayView<FBBBProjectileImpact> Impacts);
};
