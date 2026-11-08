#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBMonsterDirectorNetworkObservationProcessor.generated.h"

/** 只读取主机导演结果并交给网络桥接 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDirectorNetworkObservationProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    /** @return 构造导演观察查询 */
    UBBBMonsterDirectorNetworkObservationProcessor();
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    /** 当前导演结果只读查询 */
    FMassEntityQuery DirectorQuery;
};
