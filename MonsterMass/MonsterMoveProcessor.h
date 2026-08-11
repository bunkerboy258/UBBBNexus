#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "MonsterMoveProcessor.generated.h"

/** 批量驱动小怪朝目标坐标移动 */
UCLASS()
class ABBB_EVAC_API UMonsterMoveProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 初始化处理器执行范围与查询归属 */
    UMonsterMoveProcessor();

protected:
    /**
     * 配置小怪移动查询
     * @param EntityManager	Mass 实体管理器
     */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /**
     * 按数据块批量更新小怪位置与状态
     * @param EntityManager	Mass 实体管理器
     * @param Context		执行上下文
     */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 小怪移动实体查询 */
    FMassEntityQuery MonsterQuery;
};
