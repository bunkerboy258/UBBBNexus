#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorEventWaveDefinition.h"
#include "BBBMonsterDirectorScheduleProcessor.generated.h"

/** 推进式关卡导演调度 维护固有区激活 动态波次 预制波次与远距离回收 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDirectorScheduleProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    UBBBMonsterDirectorScheduleProcessor();

    /**
     * @param Wave	事件波次配置
     * @param Spawned	本事件已生成数量
     * @param EventAlive	本事件当前存活数量
     * @param WorldSlots	世界剩余存活额度
     * @return 本次允许提交的数量
     */
    static int32 CalculateEventRequest(const FBBBMonsterDirectorEventWaveDefinition& Wave,
        int32 Spawned, int32 EventAlive, int32 WorldSlots);
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    FMassEntityQuery DirectorQuery;
    FMassEntityQuery MonsterQuery;
};
