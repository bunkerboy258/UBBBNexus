#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "BBBMonsterAvoidanceFragment.generated.h"

/** Detour 速度避让的当前输入与输出 不单独裁决实体位置 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterAvoidanceFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 身体水平半径 */
    float CollisionRadius = 45;
    /** 邻居查询范围 */
    float NeighborSearchRadius = 400;
    /** 分离偏好强度 */
    float AvoidanceWeight = 2;
    /** 动作规则给出的期望速度 */
    FVector DesiredVelocity = FVector::ZeroVector;
    /** Detour 计算出的可行速度 */
    FVector RecommendedVelocity = FVector::ZeroVector;
    /** 近距离重叠的有界分离速度 */
    FVector SeparationVelocity = FVector::ZeroVector;
    /** 本轮预测涉及的邻居数 */
    int32 NeighborCount = 0;
    /** 本轮速度采样数 */
    int32 VelocitySamples = 0;
    /** 邻居导致推荐速度显著降低 */
    bool bCrowdConstrained = false;
};
