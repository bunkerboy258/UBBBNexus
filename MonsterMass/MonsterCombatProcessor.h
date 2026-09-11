#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterCombatProcessor.generated.h"

/** 按攻击距离与冷却批量执行小怪攻击 */
UCLASS()
class ABBB_EVAC_API UMonsterCombatProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMonsterCombatProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
