#pragma once


#include "MassVisualizationLODProcessor.h"

#include "BBBMonsterVisualizationLODProcessor.generated.h"

/** 为小怪实体计算可视距离与表现 LOD */
UCLASS()
class ABBB_EVAC_API UBBBMonsterVisualizationLODProcessor final : public UMassVisualizationLODProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪可视 LOD 处理器 */
    UBBBMonsterVisualizationLODProcessor();

protected:
    /** 将小怪标签加入各级可视查询 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
};
