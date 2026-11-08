#pragma once
#include "BBBMonsterDirectorActivation.h"
#include "BBBMonsterDirectorSpawnMode.h"
#include "BBBMonsterDirectorStageDefinition.generated.h"

class UBBBMonsterSpawnDefinition;

/** 单个副本阶段的静态生成规则 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterDirectorStageDefinition final
{
    GENERATED_BODY()

    /** 在定义资产内唯一的阶段标识 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "阶段", meta = (DisplayName = "阶段标识"))
    FName Id = TEXT("Stage01");

    /** 本阶段共用的出生规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "阶段", meta = (DisplayName = "出生配置"))
    TObjectPtr<UBBBMonsterSpawnDefinition> SpawnDefinition;

    /** 对应地图区域蓝图中的区域标识 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "阶段", meta = (DisplayName = "生成区域"))
    TArray<FName> RegionIds = {TEXT("Main")};

    /** 阶段生成规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "阶段", meta = (DisplayName = "生成方式"))
    EBBBMonsterDirectorSpawnMode Mode = EBBBMonsterDirectorSpawnMode::Waves;

    /** 阶段的启动条件 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "阶段", meta = (DisplayName = "开始条件"))
    EBBBMonsterDirectorActivation Activation = EBBBMonsterDirectorActivation::Automatic;

    /** 本阶段累计生成的有限额度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "数量", meta = (DisplayName = "总生成额度", ClampMin = "1"))
    int32 TotalCount = 20;

    /** 单次最多生成数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "数量", meta = (DisplayName = "每批数量", ClampMin = "1"))
    int32 BatchCount = 5;

    /** 当前导演全部阶段的小怪共用此存活上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "数量", meta = (DisplayName = "存活上限", ClampMin = "1"))
    int32 MaxAlive = 20;

    /** 成功生成后下一批的最短等待秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "时间", meta = (DisplayName = "生成间隔", ClampMin = "0.1", Units = "s"))
    float Interval = 10.0f;
};
