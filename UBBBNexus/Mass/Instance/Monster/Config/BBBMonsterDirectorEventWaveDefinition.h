#pragma once
#include "BBBMonsterDirectorEventWaveMode.h"
#include "BBBMonsterDirectorEventWaveDefinition.generated.h"

class UBBBMonsterSpawnDefinition;

/** 单个预制事件波次的静态配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterDirectorEventWaveDefinition final
{
    GENERATED_BODY()

    /** 在导演配置内唯一的事件标识 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "预制波次", meta = (DisplayName = "事件标识"))
    FName EventId = TEXT("Alarm01");

    /** 本事件使用的出生规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "预制波次", meta = (DisplayName = "出生配置"))
    TObjectPtr<UBBBMonsterSpawnDefinition> SpawnDefinition;

    /** 绑定的地图事件区域标识 留空时围绕存活玩家动态刷怪 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "预制波次", meta = (DisplayName = "指定区域"))
    TArray<FName> RegionIds;

    /** 爆发或持续补员模式 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "预制波次", meta = (DisplayName = "波次模式"))
    EBBBMonsterDirectorEventWaveMode Mode = EBBBMonsterDirectorEventWaveMode::Burst;

    /** Burst 为单波总量 Sustained 为总上限 零表示持续补员直到手动停止 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "数量", meta = (DisplayName = "总生成上限", ClampMin = "0"))
    int32 TotalCount = 20;

    /** 单次最多生成数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "数量", meta = (DisplayName = "每批数量", ClampMin = "1"))
    int32 BatchCount = 6;

    /** 本事件波次同时存活上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "数量", meta = (DisplayName = "事件存活上限", ClampMin = "1"))
    int32 MaxAlive = 20;

    /** 持续补员或分批提交的最小间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "时间", meta = (DisplayName = "波次间隔", ClampMin = "0.5", Units = "s"))
    float Interval = 8.0f;
};
