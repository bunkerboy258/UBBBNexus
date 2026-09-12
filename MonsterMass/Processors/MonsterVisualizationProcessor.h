#pragma once

#include "MassRepresentationProcessor.h"

#include "MonsterVisualizationProcessor.generated.h"

/** 为小怪实体创建和更新高精度 Actor 表现 */
UCLASS()
class ABBB_EVAC_API UMonsterVisualizationProcessor final : public UMassVisualizationProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪可视表现处理器 */
    UMonsterVisualizationProcessor();

protected:
    /** 将小怪标签加入表现查询 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
};
