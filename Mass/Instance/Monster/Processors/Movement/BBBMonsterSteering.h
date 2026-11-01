#pragma once

#include "CoreMinimal.h"

struct FBBBMonsterMovementFragment;

/** 移动方向连续性与身体朝向模式的共同计算规则 */
struct ABBB_EVAC_API FBBBMonsterSteering final
{
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
