#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterNavigationProcessor.generated.h"

/** 基于烘焙 NavMesh 批量驱动小怪移动 */
UCLASS()
class ABBB_EVAC_API UMonsterNavigationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMonsterNavigationProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
