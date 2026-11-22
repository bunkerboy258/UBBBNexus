#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterDirectorStatus.generated.h"

/** 导演当前状态的只读快照 供服务器蓝图与编辑器面板查询 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterDirectorStatus final
{
    GENERATED_BODY()

    /** 导演实例身份 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    FGuid InstanceId;
    /** 导演是否运行中 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    bool bRunning = false;
    /** 动态波次是否被暂停 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    bool bDynamicHordePaused = false;
    /** 当前归属导演的存活数量 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    int32 Alive = 0;
    /** 本次运行累计生成数量 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    int32 TotalSpawned = 0;
    /** 当前正在选点创建的预留数量 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    int32 Pending = 0;
    /** 已激活的固有刷怪区数量 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    int32 ActivatedPopulationRegions = 0;
    /** 地图绑定的固有刷怪区总数 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    int32 TotalPopulationRegions = 0;
    /** 距离下一次动态波次的剩余秒数 未启用或已暂停时小于零 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    float DynamicHordeCooldown = -1.0f;
    /** 当前正在运行的预制事件波次列表 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    TArray<FName> ActiveEventIds;
    /** 当前状态说明 */
    UPROPERTY(BlueprintReadOnly, Category = "Director")
    FString Reason;
};
