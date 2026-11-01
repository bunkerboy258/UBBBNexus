#pragma once

#include "CoreMinimal.h"

struct FBBBMonsterMovementFragment;
struct FCollisionQueryParams;

/** 路线净空与真实移动共用的低台阶求解 */
struct FBBBMonsterStep
{
    /**
     * @param World	碰撞世界
     * @param Body	当前姿态尺寸与跨阶高度
     * @param Start	胶囊中心起点
     * @param Delta	水平推进量
     * @param Params	当前碰撞排除规则
     * @param Center	成功时的中心落点
     * @param Normal	成功时的地面法线
     * @return 上移 前移 下落均合法且落脚高度在范围内
     */
    static bool Solve(UWorld& World, const FBBBMonsterMovementFragment& Body, const FVector& Start,
        const FVector& Delta, const FCollisionQueryParams& Params, FVector& Center, FVector& Normal);
};
