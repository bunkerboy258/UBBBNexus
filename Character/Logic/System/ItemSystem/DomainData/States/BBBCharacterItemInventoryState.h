#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Definitions/BBBCharacterItem.h"
#include "BBBCharacterItemInventoryState.generated.h"

/** 角色唯一物品位置与数量 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemInventoryState final
{
    GENERATED_BODY()
    /** 背包与身体穿戴位置共用的真实数组 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "物品位置"))
    TArray<FBBBCharacterItem> Slots;
    /** 四类背包的总格子数量 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "背包格子"))
    int32 BackpackSlotCount = 0;
    /** 装备区域内的快捷格子 */
    int32 QuickAccessSlotCount = 0;
    /** 装备区域总格子数量 */
    int32 EquipmentSlotCount = 0;
    /** 穿戴物存储区域数量 */
    int32 WearableSlotCount = 0;
    /** 消耗品区域内的快捷格子 */
    int32 ConsumableQuickAccessSlotCount = 0;
    /** 消耗品区域总格子数量 */
    int32 ConsumableSlotCount = 0;
    /** 杂物区域总格子数量 */
    int32 MiscSlotCount = 0;
    /** 背包之后的身体穿戴位置 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "穿戴位置"))
    TArray<FName> WearSlots;
    /** 位置或数量变化时递增 */
    int32 Revision = 0;
};
