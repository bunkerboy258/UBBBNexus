#pragma once

#include "CoreMinimal.h"
#include "BBBItemConfig.generated.h"

/** 角色物品存储与前序快捷选择配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemConfig final
{
    GENERATED_BODY()
    /** 全部真实物品共用的静态注册目录 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品目录"))
    TObjectPtr<class UBBBItemCatalog> Catalog = nullptr;
    /** 背包之后的穿戴物品位置 按配置顺序确定槽位 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "穿戴物品位置"))
    TArray<FName> WearSlots = {
        TEXT("Helmet"), TEXT("Body"), TEXT("Vest"), TEXT("Boots"),
        TEXT("Backpack"), TEXT("Gloves"), TEXT("Attachments"), TEXT("Flag")};

    /** 快捷格子之后存放武器与穿戴品的普通格子数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", DisplayName = "普通装备格子数量"))
    int32 EquipmentStorageSlotCount = 20;

    /** 普通装备格子之后存放杂物的格子数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", DisplayName = "杂物格子数量"))
    int32 MiscStorageSlotCount = 30;

    /** 上层物品栏使用的前序槽位数量 不增加背包容量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1", DisplayName = "前序快捷槽位数量"))
    int32 QuickAccessSlotCount = 5;
};
