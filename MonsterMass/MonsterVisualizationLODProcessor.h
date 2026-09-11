#pragma once

#include "MassVisualizationLODProcessor.h"

#include "MonsterVisualizationLODProcessor.generated.h"

/** 为小怪实体计算可视距离与表现 LOD */
UCLASS()
class ABBB_EVAC_API UMonsterVisualizationLODProcessor final : public UMassVisualizationLODProcessor
{
    GENERATED_BODY()

public:
    UMonsterVisualizationLODProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
};
