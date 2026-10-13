#pragma once
#include "MassEntityTypes.h"
#include "BBBMonsterSpawnOwnershipFragment.generated.h"

/** 小怪出生时固定的副本归属 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterSpawnOwnershipFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 创建该小怪的导演实例 */
    FGuid DirectorId;
    /** 出生时的副本运行代号 */
    uint32 Generation = 0;
    /** 出生所属阶段 */
    int32 StageIndex = INDEX_NONE;
};
