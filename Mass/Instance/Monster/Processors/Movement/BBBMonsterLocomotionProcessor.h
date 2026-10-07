#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBMonsterLocomotionProcessor.generated.h"

struct FBBBMonsterGroundFragment;

/** 统一求解真实位移与速度的主机移动处理器 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterLocomotionProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量移动处理器 */
    UBBBMonsterLocomotionProcessor();

    /**
     * @param Movement		当前档位与移动参数
     * @param RemainingDistance		到停靠位置的剩余路径长度
     * @param bPatrol		是否为固定步行巡逻
     * @return 经过边界缓冲的目标档位
     */
    static EBBBMonsterGait SelectGait(const FBBBMonsterMovementFragment& Movement, float RemainingDistance, bool bPatrol);

    /**
     * @param Movement		移动参数
     * @param CurrentSpeed		上一帧真实水平速度
     * @param TargetSpeed		当前档位目标速度
     * @param RemainingDistance		到停靠位置的剩余路径长度
     * @param DeltaSeconds		当前更新步长
     * @return 经加减速与制动距离约束的速度
     */
    static float CalculateSpeed(const FBBBMonsterMovementFragment& Movement, float CurrentSpeed, float TargetSpeed, float RemainingDistance, float DeltaSeconds);

    /**
     * @param CurrentRotation		当前逻辑朝向
     * @param HorizontalDelta		本帧主动水平位移
     * @param Velocity		碰撞后的真实速度
     * @param DeltaSeconds		当前更新步长
     * @return 过滤低速修正并限制转身速度的水平朝向
     */
    static FQuat CalculateFacing(const FQuat& CurrentRotation, const FVector& HorizontalDelta,
        const FVector& Velocity, float DeltaSeconds);

    /**
     * @param World		碰撞查询与重力来源
     * @param Movement		逻辑胶囊与地面参数
     * @param Ground		本次支撑结果
     * @param Location		逻辑中心位置
     * @param Velocity		唯一速度状态
     * @param HorizontalDelta		本帧水平移动意图
     * @param DeltaSeconds		完整更新时长
     * @return 无
     */
    static void SolveGroundMotion(UWorld& World, const FBBBMonsterMovementFragment& Movement,
        FBBBMonsterGroundFragment& Ground, FVector& Location, FVector& Velocity,
        const FVector& HorizontalDelta, float DeltaSeconds);

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 移动实体查询 */
    FMassEntityQuery MonsterQuery;
};
