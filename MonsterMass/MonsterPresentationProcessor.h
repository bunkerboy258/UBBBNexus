#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterPresentationProcessor.generated.h"

/** 将 Mass 权威状态同步给高细节小怪 Actor */
UCLASS()
class ABBB_EVAC_API UMonsterPresentationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMonsterPresentationProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
