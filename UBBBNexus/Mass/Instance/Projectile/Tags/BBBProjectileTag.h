#pragma once

#include "MassEntityTypes.h"
#include "BBBProjectileTag.generated.h"

/** 标记由ProjectileMass生命周期管理的实体弹丸 */
USTRUCT()
struct ABBB_EVAC_API FBBBProjectileTag final : public FMassTag
{
    GENERATED_BODY()
};
