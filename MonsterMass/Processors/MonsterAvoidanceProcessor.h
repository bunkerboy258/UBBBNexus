#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterAvoidanceProcessor.generated.h"

/** 使用空间分桶计算小怪的局部避让方向 */
UCLASS()
class ABBB_EVAC_API UMonsterAvoidanceProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪局部避让处理器 */
    UMonsterAvoidanceProcessor();

protected:
    /** 配置参与局部避让查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 计算邻居分离方向并修正实体位置 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;
};
