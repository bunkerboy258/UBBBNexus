#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterDirectorReplicationState.h"
#include "BBBMonsterDirectorParseProcessor.generated.h"

/** 固定阶段消费导演输入并提供只读结果查询 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDirectorParseProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    /** @return 初始化导演输入查询 */
    UBBBMonsterDirectorParseProcessor();

    /**
     * @param World	目标世界
     * @param Entity	目标导演实体
     * @param Result	当前结果副本
     * @return 是否取得有效导演
     */
    static bool Query(UWorld& World, FMassEntityHandle Entity, FBBBMonsterDirectorReplicationState& Result);
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    /** 只查询导演输入与状态 */
    FMassEntityQuery DirectorQuery;
};
