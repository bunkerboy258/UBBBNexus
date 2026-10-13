#pragma once
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterSpawnRegionEQSGenerator.generated.h"

/** 从当前副本阶段的区域生成有界候选位置 */
UCLASS(meta = (DisplayName = "副本小怪区域候选点"))
class ABBB_EVAC_API UBBBMonsterSpawnRegionEQSGenerator final : public UEnvQueryGenerator
{
    GENERATED_BODY()
public:
    /** @return 设置候选点类型 */
    UBBBMonsterSpawnRegionEQSGenerator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

    /**
     * @param Regions	阶段区域
     * @param Count	最大候选数量
     * @param Seed	本次随机种子
     * @param Result	候选纯值位置
     * @return 无返回值
     */
    static void GenerateCandidates(const TArray<FBBBMonsterSpawnRegionDefinition>& Regions, int32 Count, int32 Seed, TArray<FVector>& Result);
};
