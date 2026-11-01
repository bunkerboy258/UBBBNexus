#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBMonsterChargeProcessor.generated.h"

/** 推进冲撞与喷吐准备阶段 正常移动在技能接管时让行 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterChargeProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UBBBMonsterChargeProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
