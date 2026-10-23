#pragma once

#include "MassProcessor.h"
#include "BBBMonsterGoreProcessor.generated.h"

/** 批量转交既有事实到受伤表现 不维护第二套伤害规则 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterGoreProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** @return 注册伤害之后的表现结果维护 */
    UBBBMonsterGoreProcessor();

protected:
    /** @param EntityManager 实体管理器 @return 声明访问范围 */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** @param EntityManager 实体管理器 @param Context 调度上下文 @return 更新表现结果 */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery Query;
};
