#pragma once

#include "CoreMinimal.h"

class ANavigationData;
class AActor;
class UWorld;
struct FBBBMonsterMovementFragment;
struct FBBBMonsterNavigationFragment;

/** 导航处理器使用的当前路线几何与身体通行计算 不持有第二份运行状态 */
class FBBBMonsterPathFollowing final
{
public:
    /**
     * @param Path	唯一当前路线
     * @param Feet	真实脚底位置
     * @param StepHeight	当前姿态可跨越高度
     * @param bGrounded	是否有真实地面支撑
     * @return 是否已经到达同高度的路径终点
     */
    static bool UpdateProgress(FBBBMonsterNavigationFragment& Path, const FVector& Feet, float StepHeight, bool bGrounded);

    /**
     * @param World	物理查询世界
     * @param Navigation	当前路线使用的导航数据
     * @param Path	唯一当前路线及前瞻缓存
     * @param Feet	真实脚底位置
     * @param Geometry	当前姿态胶囊与台阶能力
     * @param LookAhead	本轮前瞻长度
     * @param IgnoredActor	自身表现载体
     * @param Now	本机世界时间
     * @return 无
     */
    static void RefreshTarget(UWorld& World, const ANavigationData& Navigation,
        FBBBMonsterNavigationFragment& Path, const FVector& Feet,
        const FBBBMonsterMovementFragment& Geometry, float LookAhead, const AActor* IgnoredActor, float Now);
};
