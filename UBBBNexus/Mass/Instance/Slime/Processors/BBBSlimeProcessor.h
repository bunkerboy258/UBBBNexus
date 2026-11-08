#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBSlimeProcessor.generated.h"

/** 推进粘液飞行落地与到期 并提供只读地面接触查询 */
UCLASS()
class ABBB_EVAC_API UBBBSlimeProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    UBBBSlimeProcessor();
    /** @param World 当前世界 @param Position 玩家接触位置 @param Radius 玩家水平半径 @param DamagePerSecond 最强伤害 @param SpeedScale 最强减速 @return 是否接触有效粘液 */
    static bool QueryExposure(UWorld& World, const FVector& Position, float Radius, float& DamagePerSecond, float& SpeedScale);
    /** @param World 当前世界 @param SourceId 喷吐者身份 @param SourceCount 同源粘液数量 @return 当前全部粘液数量 */
    static int32 Count(UWorld& World, const FGuid& SourceId, int32& SourceCount);
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    FMassEntityQuery SlimeQuery;
};
