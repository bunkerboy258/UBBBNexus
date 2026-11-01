#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBMonsterSpitProcessor.generated.h"

/** 推进喷吐前摇并在安全创建阶段递交粘液出生输入 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterSpitProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    UBBBMonsterSpitProcessor();
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    FMassEntityQuery MonsterQuery;
};
