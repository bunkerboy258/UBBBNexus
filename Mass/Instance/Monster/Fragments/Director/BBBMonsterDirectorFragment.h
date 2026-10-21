#pragma once
#include "MassEntityTypes.h"
#include "BBBMonsterDirectorPhase.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterDirectorFragment.generated.h"

class UBBBMonsterDirectorDefinition;
class ABBBMonsterDirectorPresentationActor;

/** 单个副本导演的唯一运行状态 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterDirectorFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 共享静态定义 */
    TWeakObjectPtr<UBBBMonsterDirectorDefinition> Definition;
    /** 对应的地图配置与表现桥接 */
    TWeakObjectPtr<ABBBMonsterDirectorPresentationActor> Presentation;
    /** 初始化时冻结的地图区域 */
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;
    /** 当前地图导演实例身份 */
    FGuid InstanceId;
    /** 当前入口选择一次的随机根种子 */
    uint32 Seed = 1;
    /** 每次重新开始递增的运行代号 */
    uint32 Generation = 0;
    /** 对外当前结果版本 */
    uint32 Revision = 0;
    /** 当前阶段索引 */
    int32 StageIndex = 0;
    /** 本阶段已经创建的数量 */
    int32 StageSpawned = 0;
    /** 本次运行累计创建的数量 */
    int32 TotalSpawned = 0;
    /** 当前归属导演的存活数量 */
    int32 Alive = 0;
    /** 包含尸体的归属实体数量 */
    int32 EntityCount = 0;
    /** 下一次允许生成的世界时间 */
    double NextSpawnTime = 0.0;
    /** 下一次规则采样时间 */
    double NextUpdateTime = 0.0;
    /** 当前运行阶段 */
    EBBBMonsterDirectorPhase Phase = EBBBMonsterDirectorPhase::Inactive;
    /** 暂停前的运行阶段 */
    EBBBMonsterDirectorPhase ResumePhase = EBBBMonsterDirectorPhase::Waiting;
    /** 当前等待或失败原因 */
    FString Reason;
};

/** 当前申请与配置副本需要原生构造 拷贝和析构 */
template<>
struct TMassFragmentTraits<FBBBMonsterDirectorFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
