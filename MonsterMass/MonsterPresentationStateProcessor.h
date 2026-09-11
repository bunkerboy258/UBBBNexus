#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterPresentationStateProcessor.generated.h"

/** 从小怪逻辑状态生成表现层只读快照 */
UCLASS()
class ABBB_EVAC_API UMonsterPresentationStateProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UMonsterPresentationStateProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
