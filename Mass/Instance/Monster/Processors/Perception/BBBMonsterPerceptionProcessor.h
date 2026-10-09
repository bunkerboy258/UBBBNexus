#pragma once


#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "BBBMonsterPerceptionProcessor.generated.h"

/** 批量侦察玩家并更新小怪追踪目标 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterPerceptionProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪感知处理器 */
    UBBBMonsterPerceptionProcessor();

protected:
    /** 配置参与感知查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 检查玩家是否进入小怪视野 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;

    /** 射线预算不足时从上轮结束位置继续检查 防止固定顺序饥饿 */
    int32 NextSenseEntityIndex = 0;
};
