#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterStateProcessor.generated.h"

/** 按死亡 受伤 攻击 追击的优先级统一切换小怪状态 */
UCLASS()
class ABBB_EVAC_API UMonsterStateProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** @return 初始化处理器及执行顺序 */
    UMonsterStateProcessor();

protected:
    /**
     * 声明状态切换所需的数据访问
     * @param EntityManager	实体管理器
     * @return 无返回值
     */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /**
     * 消费受伤事实并决定本帧唯一状态
     * @param EntityManager	实体管理器
     * @param Context		当前批次
     * @return 无返回值
     */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 仅查询小怪实体 */
    FMassEntityQuery MonsterQuery;
};
