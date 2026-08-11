#pragma once

#include "MassEntitySpawnDataGeneratorBase.h"

#include "MonsterSpawnPointsGenerator.generated.h"

/** 在生成器所在位置周围的圆盘内随机生成小怪出生点 */
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "Monster Random Disc"))
class ABBB_EVAC_API UMonsterSpawnPointsGenerator final : public UMassEntitySpawnDataGeneratorBase
{
    GENERATED_BODY()

public:
    /**
     * 批量生成随机出生 Transform
     * @param QueryOwner		拥有生成器的 MassSpawner
     * @param EntityTypes	待生成的实体配置
     * @param Count		出生点总数
     * @param FinishedGeneratingSpawnPointsDelegate	生成完成回调
     */
    virtual void Generate(
        UObject& QueryOwner,
        TConstArrayView<FMassSpawnedEntityType> EntityTypes,
        int32 Count,
        FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const override;

private:
    /** 随机圆盘半径，单位为厘米 */
    UPROPERTY(EditAnywhere, Category = "Monster|Spawn", meta = (ClampMin = "0.0", UIMin = "0.0"))
    float SpawnRadius = 3000.0f;
};
