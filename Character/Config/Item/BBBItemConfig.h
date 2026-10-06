#pragma once

#include "CoreMinimal.h"
#include "BBBItemConfig.generated.h"

/** 角色物品存储与前序快捷选择配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemConfig final
{
    GENERATED_BODY()

    /** 全部背包槽位数量 包含前序快捷使用槽位 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1", DisplayName = "背包槽位数量"))
    int32 InventorySlotCount = 20;

    /** 上层物品栏使用的前序槽位数量 不增加背包容量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1", DisplayName = "前序快捷槽位数量"))
    int32 QuickAccessSlotCount = 5;
};
