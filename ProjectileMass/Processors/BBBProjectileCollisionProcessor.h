#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBProjectileCollisionProcessor.generated.h"

/** 对实体弹丸执行连续碰撞、目标穿透和点伤害 */
UCLASS()
class ABBB_EVAC_API UBBBProjectileCollisionProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建弹丸碰撞处理器
     * @return 无
     */
    UBBBProjectileCollisionProcessor();

protected:
    /** @param EntityManager 当前Mass实体管理器 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /**
     * @param EntityManager	当前Mass实体管理器
     * @param Context		当前Mass处理上下文
     */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 只查询弹丸连续碰撞所需片段 */
    FMassEntityQuery ProjectileQuery;
};
