#pragma once

#include "CoreMinimal.h"

struct FBBBMonsterMovementFragment;

/** 静态制作和临近跨障共用同一套净空与落点判断 */
struct ABBB_EVAC_API FBBBMonsterTraversalGeometry
{
    /**
     * @param World	当前世界
     * @param Feet	入口脚底
     * @param Direction	接近方向
     * @param Body	逻辑身体
     * @param MaxHeight	允许高度
     * @param Params	碰撞过滤
     * @param Ledge	输出抓握边缘
     * @param Exit	输出安全落点
     * @return 是否找到完整可通行的跨障通道
     */
    static bool Find(UWorld& World, const FVector& Feet, const FVector& Direction,
        const FBBBMonsterMovementFragment& Body, float MaxHeight, const FCollisionQueryParams& Params,
        FVector& Ledge, FVector& Exit);
};
