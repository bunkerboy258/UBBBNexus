#pragma once
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnSource.h"
#include "BBBMonsterSpawnOwnershipFragment.generated.h"

/** 小怪出生时固定的副本归属与来源 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterSpawnOwnershipFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 创建该小怪的导演或生成器实例 */
    FGuid DirectorId;
    /** 出生时的副本运行代号 */
    uint32 Generation = 0;
    /** 出生调度来源 */
    EBBBMonsterSpawnSource Source = EBBBMonsterSpawnSource::Spawner;
    /** 预制波次标识 非预制波次为 None */
    FName EventId;
};
