#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/Definitions/BBBCharacterItem.h"
#include "BBBCharacterItemInventoryState.generated.h"

/** 背包与穿戴物品栏共用的唯一真实存储 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemInventoryState final
{
    GENERATED_BODY()
    /** 前序背包与后序穿戴位置 每件实例只占一个格子 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "全部物品格子"))
    TArray<FBBBCharacterItem> Slots;
    /** 背包区域大小 包含前序快捷格子 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "背包格子数量"))
    int32 BackpackSlotCount = 0;
    /** 前序快捷格子数量 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "快捷格子数量"))
    int32 QuickAccessSlotCount = 0;
    /** 快捷格子之后的普通装备区域大小 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "普通装备格子数量"))
    int32 EquipmentStorageSlotCount = 0;
    /** 后序穿戴格子的物品用途名称 与静态配置一致 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "穿戴位置"))
    TArray<FName> WearSlots;
    /** 格子内容发生变化时递增 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "物品位置版本"))
    int32 Revision = 0;
};
