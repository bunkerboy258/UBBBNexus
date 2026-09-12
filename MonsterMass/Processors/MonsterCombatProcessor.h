#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterCombatProcessor.generated.h"

/** 按攻击距离与冷却批量执行小怪攻击 */
UCLASS()
class ABBB_EVAC_API UMonsterCombatProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪战斗处理器 */
    UMonsterCombatProcessor();

protected:
    /** 配置参与战斗查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 按攻击距离和冷却时间执行攻击 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;
};
