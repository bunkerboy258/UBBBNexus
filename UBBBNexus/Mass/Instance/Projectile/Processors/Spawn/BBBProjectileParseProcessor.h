#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBProjectileParseProcessor.generated.h"

/** 消费本机枪口出生输入 */
UCLASS()
class ABBB_EVAC_API UBBBProjectileParseProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBProjectileParseProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;

};
