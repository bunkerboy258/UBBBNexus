#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterItemConfig.generated.h"

class UBBBEquipmentCatalog;
class UBBBItemDefinition;

/** 角色出生时创建的一件背包物品 */
USTRUCT(BlueprintType)
struct FBBBDefaultBackpackItem
{
    GENERATED_BODY()

    /** 用于创建物品实例的静态配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;

    /** 初始堆叠数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 StackCount = 1;

    /** 可选的快捷操作槽位 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "-1"))
    int32 QuickAccessSlot = INDEX_NONE;
};

/** 角色唯一背包及默认物品配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemConfig
{
    GENERATED_BODY()

    /** 固定背包槽位数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 BackpackSlotCount = 20;

    /** 快捷操作槽位数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 QuickAccessSlotCount = 5;

    /** 角色出生时创建的背包物品 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FBBBDefaultBackpackItem> DefaultBackpackItems;

    /** 是否默认装备第一个快捷操作物品 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bAutoEquipFirstQuickAccessItem = true;

    /** 网络装备句柄对应的静态配置表 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UBBBEquipmentCatalog> EquipmentCatalog = nullptr;
};
