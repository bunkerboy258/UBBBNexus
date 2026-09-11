#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterDamageProcessor.generated.h"

/** 消费小怪受伤事件并更新生命与状态 */
UCLASS()
class ABBB_EVAC_API UMonsterDamageProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMonsterDamageProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
