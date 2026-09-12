#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterLifecycleProcessor.generated.h"

/** 维护受击恢复与死亡回收时序 */
UCLASS()
class ABBB_EVAC_API UMonsterLifecycleProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪生命周期处理器 */
    UMonsterLifecycleProcessor();

protected:
    /** 配置参与生命周期查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 处理受伤恢复和死亡回收 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;
};
