#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterTargetAcquisitionMode.generated.h"

/** 当前没有目标时使用的索敌规则 */
UENUM(BlueprintType)
enum class EBBBMonsterTargetAcquisitionMode : uint8
{
    ActivePerception UMETA(DisplayName = "主动感知及受击索敌"),
    RetaliationOnly UMETA(DisplayName = "受击索敌")
};
