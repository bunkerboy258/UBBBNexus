#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentReloadRuntimeData.generated.h"

/** 单件装备的换弹持续状态 */
USTRUCT()
struct FBBBEquipmentReloadRuntimeData
{
    GENERATED_BODY()

    bool bIsReloading = false;
    float StartTimeSeconds = 0.0f;
    float DurationSeconds = 0.0f;
    int32 Sequence = 0;
};
