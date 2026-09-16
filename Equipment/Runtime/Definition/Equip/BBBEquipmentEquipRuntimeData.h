#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentEquipRuntimeData.generated.h"

/** 装备挂接后采集的固定姿态数据 */
USTRUCT()
struct FBBBEquipmentEquipRuntimeData
{
    GENERATED_BODY()

    FTransform AimSourceRightHandBoneSpace = FTransform::Identity;
    bool bHasValidAimSource = false;
};
