#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterItemConfig.generated.h"

/** 角色对外侧物品定义的存储包装配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemConfig final
{
    GENERATED_BODY()

    /** 角色允许获得的装备 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备目录"))
    TObjectPtr<class UBBBEquipmentCatalog> EquipmentCatalog = nullptr;

    /** 角色允许获得的穿戴物 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "穿戴物目录"))
    TObjectPtr<class UBBBWearableCatalog> WearableCatalog = nullptr;

    /** 角色允许获得的消耗品 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "消耗品目录"))
    TObjectPtr<class UBBBConsumableCatalog> ConsumableCatalog = nullptr;

    /** 角色允许获得的杂物 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "杂物目录"))
    TObjectPtr<class UBBBMiscCatalog> MiscCatalog = nullptr;

    /** 背包之外的身体穿戴位置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "穿戴位置"))
    TArray<FName> WearSlots = {TEXT("Helmet"), TEXT("Body"), TEXT("Vest"), TEXT("Boots"),
        TEXT("Backpack"), TEXT("Gloves"), TEXT("Attachments"), TEXT("Flag"), TEXT("Legs"), TEXT("Belt")};

    /** 包含装备快捷位置的总容量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备格子", ClampMin = "5"))
    int32 EquipmentSlotCount = 20;

    /** 不含身体穿戴位置的存储容量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "穿戴物格子", ClampMin = "0"))
    int32 WearableSlotCount = 20;

    /** 包含消耗品快捷位置的总容量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "消耗品格子", ClampMin = "5"))
    int32 ConsumableSlotCount = 30;

    /** 杂物存储容量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "杂物格子", ClampMin = "0"))
    int32 MiscSlotCount = 30;

    /** 装备区域内的快捷位置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备快捷格子", ClampMin = "1"))
    int32 QuickAccessSlotCount = 5;

    /** 消耗品区域内的快捷位置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "消耗品快捷格子", ClampMin = "1"))
    int32 ConsumableQuickAccessSlotCount = 5;
};
