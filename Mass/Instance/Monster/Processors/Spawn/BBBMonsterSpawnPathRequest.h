#pragma once
#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "NavigationData.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnPoint.h"
#include "BBBMonsterSpawnPathRequest.generated.h"

class UNavigationSystemV1;
class UBBBMonsterSpawnDefinition;

/** 当前出生候选的异步可达性回执 不创建实体 */
UCLASS(Transient)
class UBBBMonsterSpawnPathRequest final : public UObject
{
    GENERATED_BODY()
public:
    /** @param Navigation 当前导航服务 @param NavData 当前导航数据 @return 无 只投递一次查询 */
    void Start(UNavigationSystemV1 &Navigation, const ANavigationData &NavData);
    /** 请求所属的完整入口身份 */
    FMassEntityHandle Entity;
    /** 入口业务身份 */
    FGuid OwnerId;
    /** 当前出生请求序号 */
    uint32 RequestId = 0;
    /** 当前导演运行代号 */
    uint32 Generation = 0;
    /** 当前导演阶段 */
    int32 StageIndex = 0;
    /** 当前请求使用的配置 */
    TWeakObjectPtr<UBBBMonsterSpawnDefinition> Definition;
    /** 等待可达性确认的单个候选 */
    FBBBMonsterSpawnPoint Point;
    /** 候选脚底 */
    FVector Location = FVector::ZeroVector;
    /** 本候选冻结的观察位置 */
    TArray<FVector> Observers;
    /** 当前正在查询的观察位置 */
    int32 ObserverIndex = 0;
    /** 当前投影终点 */
    FVector Goal = FVector::ZeroVector;
    /** 当前原生查询编号 */
    uint32 QueryId = 0;
    /** 查询使用的导航数据 */
    TWeakObjectPtr<const ANavigationData> NavigationData;
    /** 最近回执路径 */
    FNavPathSharedPtr Path;
    /** 最近原生查询结果 */
    ENavigationQueryResult::Type Result = ENavigationQueryResult::Invalid;
    /** 当前查询回执是否到达 */
    bool bComplete = false;
};
