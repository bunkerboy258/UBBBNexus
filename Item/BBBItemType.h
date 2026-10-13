#pragma once
#include "CoreMinimal.h"
#include "BBBItemType.generated.h"

/** 真实物品的静态分类 */
UENUM(BlueprintType)
enum class EBBBItemType : uint8
{
    /** 主动使用的装备 */
    Equipment UMETA(DisplayName = "装备"),
    /** 身体穿戴物 */
    Wearable UMETA(DisplayName = "穿戴物"),
    /** 使用后消耗数量的物品 */
    Consumable = 3 UMETA(DisplayName = "消耗品"),
    /** 不穿戴也不主动持有的物品 */
    Misc = 2 UMETA(DisplayName = "杂物")
};
