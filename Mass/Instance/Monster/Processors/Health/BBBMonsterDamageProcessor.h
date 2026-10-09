#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBMonsterDamageProcessor.generated.h"

struct FBBBMonsterDamageContribution;
struct FBBBMonsterBodyPartDefinition;

/** 根据累计贡献计算本机生命结果 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDamageProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBMonsterDamageProcessor();

    /**
     * 公开值查询 合并已应用与待解析快照 不暴露 Fragment
     * @param World	目标所属世界
     * @param Entity	目标小怪实体
     * @param Result	累计贡献的值快照
     * @return 是否取得有效目标的快照
     */
    static bool Query(UWorld& World, FMassEntityHandle Entity, TArray<FBBBMonsterDamageContribution>& Result);

    /**
     * @param World	目标世界
     * @param Entity	目标实体
     * @param Part	部位编号
     * @param Result	部位静态参数的值副本
     * @return 是否取得有效配置
     */
    static bool QueryPart(UWorld& World, FMassEntityHandle Entity, uint8 Part, FBBBMonsterBodyPartDefinition& Result);

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;

};
