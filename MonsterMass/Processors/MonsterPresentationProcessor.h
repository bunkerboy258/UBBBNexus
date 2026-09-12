#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterPresentationProcessor.generated.h"

/** 将 Mass 权威状态同步给高细节小怪 Actor */
UCLASS()
class ABBB_EVAC_API UMonsterPresentationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪表现同步处理器 */
    UMonsterPresentationProcessor();

protected:
    /** 配置参与表现同步查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 将 Mass 数据同步到表现 Actor */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;
};
