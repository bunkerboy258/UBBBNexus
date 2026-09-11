#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterAvoidanceProcessor.generated.h"

/** 使用空间分桶计算小怪的局部避让方向 */
UCLASS()
class ABBB_EVAC_API UMonsterAvoidanceProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMonsterAvoidanceProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
