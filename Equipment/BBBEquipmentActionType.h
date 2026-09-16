#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentActionType.generated.h"

/** 装备自身可执行的操作类型 */
UENUM(BlueprintType)
enum class EBBBEquipmentActionType : uint8
{
    None,
    Equip,
    Fire,
    Reload
};
