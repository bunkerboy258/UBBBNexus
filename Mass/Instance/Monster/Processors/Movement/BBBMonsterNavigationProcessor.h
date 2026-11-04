#pragma once


#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "BBBMonsterNavigationProcessor.generated.h"

/** 按世界预算构建 Detour 走廊并持续更新前视目标 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterNavigationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建小怪导航处理器 */
    UBBBMonsterNavigationProcessor();

protected:
    /** 配置参与导航查询的实体数据 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** 刷新导航路径并推进小怪实体 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
    virtual void BeginDestroy() override;

private:
    /** 查询全部小怪实体的 Mass 查询器 */
    FMassEntityQuery MonsterQuery;

    /** 当前世界未消费的原生异步请求 有界于查询预算 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<class UBBBMonsterNavigationRequest>> PendingQueries;

    /** 世界共享查询器 不缓存实体位置 */
    TSharedPtr<class dtNavMeshQuery> NavQuery;
};
