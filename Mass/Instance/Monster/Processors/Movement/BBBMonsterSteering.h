#pragma once

#include "CoreMinimal.h"

struct FBBBMonsterMovementFragment;
class UBBBMonsterDefinition;

/** 移动方向连续性与身体朝向模式的共同计算规则 */
struct ABBB_EVAC_API FBBBMonsterSteering final
{
    /**
     * @param GoalDelta		当前前视目标相对位置
     * @param Rule		共享逻辑配置
     * @param InstanceId		本代实体身份 决定固定的个体差异
     * @param Now		当前世界时刻
     * @param RemainingDistance		剩余路线长度
     * @param ArrivalDistance		停止曲线的到达或攻击距离
     * @param BodyRadius		接近拐角时的收敛尺度
     * @param bRunning		当前使用跑步或冲刺步态
     * @return 保持单位长度的水平期望方向 随后仍需经过避障与导航约束
     */
    static FVector ApplyTravelCurve(const FVector& GoalDelta, const UBBBMonsterDefinition& Rule,
        const FGuid& InstanceId, float Now, float RemainingDistance, float ArrivalDistance,
        float BodyRadius, bool bRunning);

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
