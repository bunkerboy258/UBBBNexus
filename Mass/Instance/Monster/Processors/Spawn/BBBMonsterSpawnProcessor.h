#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterSpawnProcessor.generated.h"

class UBBBMonsterSpawnDefinition;
class UBBBMonsterDefinition;

/** 所有生成入口共用的有界选点与安全创建 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterSpawnProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    /** @return 构造统一出生查询 无返回值 */
    UBBBMonsterSpawnProcessor();

    /**
     * @param Seed	请求种子
     * @param Index	候选序号
     * @param Channel	独立随机通道
     * @return 非零的确定性随机位
     */
    static uint32 RandomBits(uint32 Seed, uint32 Index, uint32 Channel);

    /**
     * @param Regions	区域快照
     * @param Seed	请求种子
     * @param Index	候选序号
     * @return 确定性候选位置
     */
    static FVector Candidate(const TArray<FBBBMonsterSpawnRegionDefinition>& Regions, uint32 Seed, int32 Index);

    /**
     * @param Definition	出生配置
     * @param Seed	请求种子
     * @param Index	候选序号
     * @return 带权重选中的模板
     */
    static UBBBMonsterDefinition* SelectDefinition(const UBBBMonsterSpawnDefinition& Definition, uint32 Seed, int32 Index);

    /**
     * @param World	检查世界
     * @param Definition	出生规则
     * @param Regions	区域快照
     * @param Candidates	候选位置
     * @param Observers	玩家或预览参考位置
     * @param Directions	对应观察方向
     * @param Occupied	已占位置
     * @param Count	所需数量
     * @param Result	合格地面位置
     * @param Rejected	拒绝位置
     * @param Reasons	对应拒绝原因
     * @return 无返回值
     */
    static void Filter(UWorld& World, const UBBBMonsterSpawnDefinition& Definition,
        const TArray<FBBBMonsterSpawnRegionDefinition>& Regions, const TArray<FVector>& Candidates,
        const TArray<FVector>& Observers, const TArray<FVector>& Directions, const TArray<FVector>& Occupied,
        int32 Count, TArray<FVector>& Result, TArray<FVector>& Rejected, TArray<FString>& Reasons);
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    FMassEntityQuery SpawnQuery;
    FMassEntityQuery MonsterQuery;
};
