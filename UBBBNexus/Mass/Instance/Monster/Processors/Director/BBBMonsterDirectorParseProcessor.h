#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorStatus.h"
#include "BBBMonsterDirectorParseProcessor.generated.h"

/** 解析导演输入 同步下游刷怪回执并提供只读查询 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDirectorParseProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    UBBBMonsterDirectorParseProcessor();

    /**
     * @param World	目标世界
     * @param Entity	导演实体句柄
     * @param Result	输出的只读状态快照
     * @return 是否成功读取
     */
    static bool Query(UWorld& World, FMassEntityHandle Entity, FBBBMonsterDirectorStatus& Result);
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    FMassEntityQuery DirectorQuery;
};
