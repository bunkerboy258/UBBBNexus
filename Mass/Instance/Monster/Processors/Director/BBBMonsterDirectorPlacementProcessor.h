#pragma once
#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterDirectorPlacementProcessor.generated.h"

class UBBBMonsterDirectorDefinition;

/** 主机出生选点与安全批量创建 预览复用同一位置检查 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterDirectorPlacementProcessor final : public UMassProcessor
{
    GENERATED_BODY()
public:
    /** @return 构造当前申请与世界小怪查询 */
    UBBBMonsterDirectorPlacementProcessor();

    /**
     * @param World	当前世界
     * @param Entity	导演实体
     * @param StageIndex	当前阶段
     * @param Result	冻结的区域配置副本
     * @return 是否取得指定阶段区域
     */
    static bool QueryRegions(UWorld& World, FMassEntityHandle Entity, int32 StageIndex, TArray<FBBBMonsterSpawnRegionDefinition>& Result);

    /**
     * @param World	检查世界
     * @param Definition	副本规则
     * @param StageIndex	检查阶段
     * @param Regions	有效区域
     * @param Candidates	候选位置
     * @param ObserverPositions	玩家或编辑器预览参考位置
     * @param ObserverDirections	观察朝向
     * @param Occupied	已经占用或预留的出生位置
     * @param Count	本批最大数量
     * @param Result	通过检查的位置
     * @param Rejected	被拒绝的位置
     * @param Reasons	与拒绝位置对应的原因
     * @return 无返回值
     */
    static void Filter(UWorld& World, const UBBBMonsterDirectorDefinition& Definition, int32 StageIndex,
        const TArray<FBBBMonsterSpawnRegionDefinition>& Regions, const TArray<FVector>& Candidates,
        const TArray<FVector>& ObserverPositions, const TArray<FVector>& ObserverDirections,
        const TArray<FVector>& Occupied, int32 Count, TArray<FVector>& Result, TArray<FVector>& Rejected, TArray<FString>& Reasons);
protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
private:
    /** 当前副本申请 */
    FMassEntityQuery DirectorQuery;
    /** 当前世界存活小怪的位置 */
    FMassEntityQuery MonsterQuery;
};
