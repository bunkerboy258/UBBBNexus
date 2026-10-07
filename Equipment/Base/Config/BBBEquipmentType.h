#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentType.generated.h"

/** 当前装备配置的实际类别 */
UENUM(BlueprintType)
enum class EBBBEquipmentType : uint8
{
    /** 未配置或没有装备 */
    None UMETA(DisplayName = "无"),
    /** 使用枪口瞄准的步枪 */
    Rifle UMETA(DisplayName = "步枪"),
    /** 不使用枪口瞄准的近战装备 */
    Melee UMETA(DisplayName = "近战")
};
