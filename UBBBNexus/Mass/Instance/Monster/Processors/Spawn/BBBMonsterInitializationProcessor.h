#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBMonsterInitializationProcessor.generated.h"

class UBBBMonsterDefinition;
struct FBBBMonsterVariationFragment;
struct FBBBMonsterMovementFragment;
struct FBBBMonsterBehaviorFragment;

/** 批量完成出生属性初始化 只查询待初始化实体 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterInitializationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** @return 构造出生查询 无返回值 */
    UBBBMonsterInitializationProcessor();

    /**
     * @param Identity	稳定出生身份
     * @param Definition	共享静态配置
     * @param Variation	固定个体特征
     * @param Movement	最终个体移动参数
     * @param Behavior	最终个体行为参数
     * @return 是否完成确定性初始化
     */
    static bool InitializeBirth(const FGuid& Identity, const UBBBMonsterDefinition& Definition,
        FBBBMonsterVariationFragment& Variation, FBBBMonsterMovementFragment& Movement, FBBBMonsterBehaviorFragment& Behavior);

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 仅包含出生待处理实体的查询 */
    FMassEntityQuery PendingQuery;
};
