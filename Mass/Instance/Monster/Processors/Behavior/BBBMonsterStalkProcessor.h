#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBMonsterStalkProcessor.generated.h"

/** 有界选择玩家观察之外的可达接近点 并维护突袭与狂攻阶段 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterStalkProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    UBBBMonsterStalkProcessor();

    /** @param World 查询世界 @param Point 候选位置 @param Ignored 表现载体 @return 是否被任意玩家直接观察 */
    static bool IsObserved(UWorld& World, const FVector& Point, const AActor* Ignored);

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery MonsterQuery;
};
