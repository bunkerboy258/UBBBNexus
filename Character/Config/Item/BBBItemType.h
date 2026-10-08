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
    /** 手套 附件与国旗 */
    Misc UMETA(DisplayName = "杂物")
};
