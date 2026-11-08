#pragma once

#include "MassProcessor.h"
#include "BBBMonsterHitReactionProcessor.generated.h"

/** 批量推进命中间隔并提交一次当前血效 不改变伤害和移动 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterHitReactionProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** @return 配置调度 无返回值 */
    UBBBMonsterHitReactionProcessor();

protected:
    /**
     * @param EntityManager	批量实体管理器
     * @return 无返回值
     */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    /**
     * @param EntityManager	批量实体管理器
     * @param Context	当前帧批量上下文
     * @return 无返回值
     */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;
};
