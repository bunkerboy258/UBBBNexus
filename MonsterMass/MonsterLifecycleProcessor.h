#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterLifecycleProcessor.generated.h"

/** 维护受击恢复与死亡回收时序 */
UCLASS()
class ABBB_EVAC_API UMonsterLifecycleProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMonsterLifecycleProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
