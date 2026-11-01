#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterAvoidanceFragment.generated.h"

/** 小怪局部避让配置与运行状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterAvoidanceFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 小怪球形碰撞体半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "碰撞半径"))
    float CollisionRadius = 45.0f;

    /** 小怪之间保持的最小中心距离 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "个人空间半径"))
    float PersonalSpaceRadius = 110.0f;

    /** 参与局部避让计算的邻居范围 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "邻近搜索半径"))
    float NeighborSearchRadius = 180.0f;

    /** 避让方向对追击方向的影响权重 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "避让权重"))
    float AvoidanceWeight = 2.0f;

    /** 当前帧计算出的远离邻居方向 */
    FVector SeparationDirection = FVector::ZeroVector;

    /** 当前帧计算出的避让强度 */
    float SeparationStrength = 0.0f;

    /** 绕行一侧的最短保持时长 */
    float SideHoldDuration = 0.75f;

    /** 相对路线的当前绕行侧 零表示尚未选择 */
    int8 SteeringSide = 0;

    /** 当前绕行侧允许重新评分的时刻 */
    float SideHeldUntil = 0.0f;

    /** 已确认的绕行换边次数 用于连续运行验收 */
    uint32 SideChanges = 0;

    /** 当前邻居间距不足以安全推进 等待空间恢复 */
    bool bWaitingForNeighbor = false;

    /** 通过邻居身体净空检查的当前移动方向 */
    FVector SteeringDirection = FVector::ZeroVector;

    /** 当前方向允许推进的厘米数 移动处理器仍检查环境碰撞 */
    float MaxTravelDistance = 0.0f;

    /** 当前可行推进被活体邻居收窄 不用于判定环境不可达 */
    bool bCrowdConstrained = false;
};
