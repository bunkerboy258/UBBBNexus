#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBMonsterLocomotionProcessor.generated.h"

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

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 移动实体查询 */
    FMassEntityQuery MonsterQuery;
};
