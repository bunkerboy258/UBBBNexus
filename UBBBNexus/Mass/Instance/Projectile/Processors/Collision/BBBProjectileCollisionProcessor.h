#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBProjectileCollisionProcessor.generated.h"

/** 本机弹全量造伤碰撞 远端弹按距离预算只发击中特效 */
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

    /** 近中远三档游标 下一帧从该下标继续 */
    int32 TierCursor[3] = {};
};
