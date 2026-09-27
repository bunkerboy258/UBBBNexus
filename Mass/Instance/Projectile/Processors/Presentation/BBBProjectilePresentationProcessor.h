#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBProjectilePresentationProcessor.generated.h"

/** 按通道批量发布子弹光效事实 */
UCLASS()
class ABBB_EVAC_API UBBBProjectilePresentationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBProjectilePresentationProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;

};
