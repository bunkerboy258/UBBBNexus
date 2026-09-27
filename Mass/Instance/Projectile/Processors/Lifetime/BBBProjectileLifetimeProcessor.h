#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBProjectileLifetimeProcessor.generated.h"

/** 安全回收结束的子弹实体 */
UCLASS()
class ABBB_EVAC_API UBBBProjectileLifetimeProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBProjectileLifetimeProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;

};
