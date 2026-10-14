#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBMonsterNetworkObservationProcessor.generated.h"

/** 观察实体当前事实并交给网络载体 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterNetworkObservationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBMonsterNetworkObservationProcessor();

    /**
     * @param World\t所属世界
     * @param Entity\t本机实体句柄
     * @param Result\t跨机出生身份
     * @return 是否取得有效身份
     */
    static bool QueryIdentity(UWorld& World, FMassEntityHandle Entity, FGuid& Result);

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;
    /** 发送节流 不是玩法时间 */
    float NextPublishTime = 0.0f;
};
