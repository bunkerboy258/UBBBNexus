#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterInitializationPendingTag.generated.h"

/** 只筛选尚未获得出生属性的实体 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterInitializationPendingTag final : public FMassTag
{
    GENERATED_BODY()
};
