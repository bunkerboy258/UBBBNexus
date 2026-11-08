#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBProjectileCollisionProcessor.generated.h"

/** 比较世界碰撞与逻辑实体碰撞并提交合法伤害 */
UCLASS()
class ABBB_EVAC_API UBBBProjectileCollisionProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBProjectileCollisionProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;

};
