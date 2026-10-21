#pragma once
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterSpawnPoint.h"
#include "BBBMonsterSpawnFragment.generated.h"

class UBBBMonsterSpawnDefinition;

/** 每个入口只有一个当前出生请求 不保存请求历史 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterSpawnFragment final : public FMassFragment
{
    GENERATED_BODY()
    /** 入口身份 */
    FGuid OwnerId;
    /** 请求序号 旧的延迟创建不能写入新请求 */
    uint32 RequestId = 0;
    /** 导演运行代号 独立生成器同样使用递增代号 */
    uint32 Generation = 0;
    /** 所属导演阶段 独立生成使用零 */
    int32 StageIndex = 0;
    /** 唯一的出生配置 */
    TWeakObjectPtr<UBBBMonsterSpawnDefinition> Definition;
    /** 本请求冻结的区域 */
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;
    /** 请求级随机种子 */
    uint32 Seed = 1;
    /** 请求数量 */
    int32 Requested = 0;
    /** 已实际创建数量 取消也不清零 */
    int32 Created = 0;
    /** 下一个候选序号 */
    int32 Cursor = 0;
    /** 累计已通过位置检查的数量 */
    int32 Accepted = 0;
    /** 请求是否仍允许工作 */
    bool bActive = false;
    /** 请求是否暂时暂停 */
    bool bPaused = false;
    /** 已排入安全时机的创建不得重复入队 */
    bool bCreating = false;
    /** 还未创建的合格出生点 */
    TArray<FBBBMonsterSpawnPoint> Points;
    /** 按原因统计的拒绝数量 */
    TMap<FString, int32> Rejections;
    /** 当前结果说明 */
    FString Reason;
};

template<>
struct TMassFragmentTraits<FBBBMonsterSpawnFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
