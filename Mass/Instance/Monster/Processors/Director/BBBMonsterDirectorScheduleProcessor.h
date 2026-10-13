#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBMonsterDirectorScheduleProcessor.generated.h"

struct FBBBMonsterDirectorStageDefinition;

/** 主机统计归属并调度有界出生申请 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDirectorScheduleProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    /** @return 初始化导演与小怪查询 */
    UBBBMonsterDirectorScheduleProcessor();

    /**
     * @param Stage	阶段静态规则
     * @param Spawned	当前阶段实际生成数量
     * @param Alive	当前导演存活数量
     * @param WorldSlots	世界扣除存活与预留后的余量
     * @return 本次允许申请的数量
     */
    static int32 CalculateRequest(const FBBBMonsterDirectorStageDefinition& Stage, int32 Spawned, int32 Alive, int32 WorldSlots);
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    /** 导演规则与预留量 */
    FMassEntityQuery DirectorQuery;
    /** 当前世界小怪与归属 */
    FMassEntityQuery MonsterQuery;
};
