#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBMonsterDirectorSpawnInitializationProcessor.generated.h"

/** 创建批次中写入位置与固定归属 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDirectorSpawnInitializationProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    /** @return 构造非自动运行的出生初始化查询 */
    UBBBMonsterDirectorSpawnInitializationProcessor();
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    /** 当前创建批次的小怪 */
    FMassEntityQuery SpawnQuery;
};
