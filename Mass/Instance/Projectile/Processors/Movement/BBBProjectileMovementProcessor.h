#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBProjectileMovementProcessor.generated.h"

/** 批量积分子弹位置 */
UCLASS()
class ABBB_EVAC_API UBBBProjectileMovementProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBProjectileMovementProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;

};
