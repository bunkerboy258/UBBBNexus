#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterPerceptionProcessor.generated.h"

/** 批量侦察玩家并更新小怪追踪目标 */
UCLASS()
class ABBB_EVAC_API UMonsterPerceptionProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMonsterPerceptionProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
