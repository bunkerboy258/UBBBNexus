#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterPresentationStateProcessor.generated.h"

/** 从小怪逻辑状态生成表现层只读快照 */
UCLASS()
class ABBB_EVAC_API UMonsterPresentationStateProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪表现状态处理器 */
    UMonsterPresentationStateProcessor();

protected:
    /** 配置参与表现状态查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 生成供表现层读取的状态快照 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;
};
