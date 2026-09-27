#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBProjectileLifetimeProcessor.generated.h"

/** 按弹丸存活时限回收实体并消费碰撞回收标记 */
UCLASS()
class ABBB_EVAC_API UBBBProjectileLifetimeProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建弹丸寿命处理器
     * @return 无
     */
    UBBBProjectileLifetimeProcessor();

protected:
    /** @param EntityManager 当前Mass实体管理器 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /**
     * @param EntityManager	当前Mass实体管理器
     * @param Context		当前Mass处理上下文
     */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 只查询弹丸寿命所需片段 */
    FMassEntityQuery ProjectileQuery;
};
