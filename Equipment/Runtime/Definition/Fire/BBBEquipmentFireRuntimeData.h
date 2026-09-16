#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentFireRuntimeData.generated.h"

/** 单件装备的开火时间状态 */
USTRUCT()
struct FBBBEquipmentFireRuntimeData
{
    GENERATED_BODY()

    float LastFireTimeSeconds = -1000.0f;
};
