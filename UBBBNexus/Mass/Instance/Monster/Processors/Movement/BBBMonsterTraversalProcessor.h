#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterTraversalFact.h"
#include "BBBMonsterTraversalProcessor.generated.h"

/** 按共享预算进入台沿并批量推进唯一的攀爬位移 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterTraversalProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UBBBMonsterTraversalProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
