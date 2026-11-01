#pragma once

#include "CoreMinimal.h"

struct FBBBMonsterMovementFragment;

/** 移动方向连续性与身体朝向模式的共同计算规则 */
struct ABBB_EVAC_API FBBBMonsterSteering final
{
    /**
     * @param Velocity		上一轮真实水平速度
     * @param Direction		本轮候选水平意图
     * @param Acceleration		水平加速度上限
     * @param TurnRate		身体转速上限
     * @param DeltaSeconds		本轮步长
     * @return 与运动连续的候选方向
     */
    static FVector ConstrainDirection(const FVector& Velocity, const FVector& Direction,
        float Acceleration, float TurnRate, float DeltaSeconds);

    /**
     * @param Movement		移动处理器持有的朝向模式
     * @param bHasTarget		当前目标有效
     * @param bForceTarget		警觉或攻击要求面向目标
     * @param bChasing		当前正在接近目标
     * @param bCrowdStationary		持续拥挤且实际低速
     * @param Distance		目标水平距离
     * @param AttackRange		近战接近范围
     * @param BodyRadius		身体半径形成的退出缓冲
     * @param Now		当前世界时刻
     * @return 无
     */
    static void UpdateFacingMode(FBBBMonsterMovementFragment& Movement, bool bHasTarget,
        bool bForceTarget, bool bChasing, bool bCrowdStationary, float Distance,
        float AttackRange, float BodyRadius, float Now);
};
