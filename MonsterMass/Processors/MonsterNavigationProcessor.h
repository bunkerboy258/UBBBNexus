#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterNavigationProcessor.generated.h"

/** 基于烘焙 NavMesh 批量驱动小怪移动 */
UCLASS()
class ABBB_EVAC_API UMonsterNavigationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪导航处理器 */
    UMonsterNavigationProcessor();

protected:
    /** 配置参与导航查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 刷新导航路径并推进小怪实体 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;
};
