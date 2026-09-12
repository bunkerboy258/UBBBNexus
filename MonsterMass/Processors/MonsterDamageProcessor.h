#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterDamageProcessor.generated.h"

/** 消费小怪受伤事件并更新生命与状态 */
UCLASS()
class ABBB_EVAC_API UMonsterDamageProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪受伤处理器 */
    UMonsterDamageProcessor();

protected:
    /** 配置参与受伤查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 消费待处理伤害并更新生命状态 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;
};
