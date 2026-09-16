#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentReloadRuntimeData.generated.h"

/** 单件装备的换弹持续状态 */
USTRUCT()
struct FBBBEquipmentReloadRuntimeData
{
    GENERATED_BODY()

    bool bIsReloading = false;
    bool bMagazineDetached = false;
    int32 Sequence = 0;
};
