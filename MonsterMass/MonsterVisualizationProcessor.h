#pragma once

#include "MassRepresentationProcessor.h"

#include "MonsterVisualizationProcessor.generated.h"

/** 为小怪实体创建和更新高精度 Actor 表现 */
UCLASS()
class ABBB_EVAC_API UMonsterVisualizationProcessor final : public UMassVisualizationProcessor
{
    GENERATED_BODY()

public:
    UMonsterVisualizationProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
};
