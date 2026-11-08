#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterTag.generated.h"

/** 标记实体属于小怪 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterTag final : public FMassTag
{
    GENERATED_BODY()
};
