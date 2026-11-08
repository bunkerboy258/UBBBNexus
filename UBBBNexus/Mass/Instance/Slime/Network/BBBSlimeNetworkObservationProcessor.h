#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBSlimeNetworkObservationProcessor.generated.h"

/** 按固定频率发布有界粘液当前集合 */
UCLASS()
class ABBB_EVAC_API UBBBSlimeNetworkObservationProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    UBBBSlimeNetworkObservationProcessor();
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    FMassEntityQuery SlimeQuery;
    /** 世界网络观察的下一次调度时刻 */
    float NextPublishTime = 0;
};
