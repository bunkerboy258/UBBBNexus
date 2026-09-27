#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBProjectileMovementProcessor.generated.h"

/** 批量积分实体弹丸的匀速弹道位置 */
UCLASS()
class ABBB_EVAC_API UBBBProjectileMovementProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建弹丸移动处理器
     * @return 无
     */
    UBBBProjectileMovementProcessor();

protected:
    /** @param EntityManager 当前Mass实体管理器 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /**
     * @param EntityManager	当前Mass实体管理器
     * @param Context		当前Mass处理上下文
     */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 只查询弹丸移动所需片段 */
    FMassEntityQuery ProjectileQuery;
};
