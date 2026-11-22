#pragma once
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnSource.h"
#include "BBBMonsterDirectorFragment.generated.h"

class UBBBMonsterDirectorDefinition;
class UBBBMonsterSpawnDefinition;
class ABBBMonsterDirectorPresentationActor;

/** 导演内正在运行的单个预制事件波次状态 */
struct FBBBMonsterDirectorActiveEvent final
{
    FName EventId;
    int32 Spawned = 0;
    double NextSpawnTime = 0.0;
    int32 InitialTargetPlayerId = INDEX_NONE;
};

/** 导演内部等待提交给刷怪机的请求项 */
struct FBBBMonsterDirectorQueuedRequest final
{
    EBBBMonsterSpawnSource Source = EBBBMonsterSpawnSource::Population;
    FName EventId;
    TWeakObjectPtr<UBBBMonsterSpawnDefinition> Definition;
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;
    int32 Count = 0;
    int32 InitialTargetPlayerId = INDEX_NONE;
    bool bAlertOnSpawn = false;
};

/** 单个关卡导演的唯一运行状态 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterDirectorFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 共享静态定义 */
    TWeakObjectPtr<UBBBMonsterDirectorDefinition> Definition;
    /** 对应的地图配置桥接 */
    TWeakObjectPtr<ABBBMonsterDirectorPresentationActor> Presentation;
    /** 初始化时冻结的可刷区域 包含固有区与事件区 */
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;
    /** 初始化时冻结的禁刷区域 */
    TArray<FBBBMonsterSpawnRegionDefinition> ExcludeRegions;
    /** 已触发过的固有刷怪区标识 防止重复铺怪 */
    TSet<FName> ActivatedPopulationRegionIds;
    /** 当前正在运行的预制事件波次 */
    TArray<FBBBMonsterDirectorActiveEvent> ActiveEvents;
    /** 导演内部短队列 按优先级依次喂给刷怪机 */
    TArray<FBBBMonsterDirectorQueuedRequest> Queue;
    /** 当前地图导演实例身份 */
    FGuid InstanceId;
    /** 当前入口选择一次的随机根种子 */
    uint32 Seed = 1;
    /** 每次重新开始递增的运行代号 */
    uint32 Generation = 0;
    /** 动态波次随机序号 */
    uint32 DynamicHordeCounter = 0;
    /** 导演是否正在运行 */
    bool bRunning = false;
    /** 是否手动暂停动态波次 */
    bool bDynamicHordePaused = false;
    /** 本次运行累计创建的数量 */
    int32 TotalSpawned = 0;
    /** 当前归属导演的存活数量 */
    int32 Alive = 0;
    /** 下一次触发动态波次的世界时间 */
    double NextDynamicHordeTime = 0.0;
    /** 下一次规则采样时间 */
    double NextUpdateTime = 0.0;
    /** 当前状态或等待原因 */
    FString Reason;
};

/** 当前状态包含容器成员 需要原生构造 拷贝和析构 */
template<>
struct TMassFragmentTraits<FBBBMonsterDirectorFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
