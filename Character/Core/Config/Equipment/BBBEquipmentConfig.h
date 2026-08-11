#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentConfig.generated.h"

class UBBBEquipmentCatalog;
class UBBBEquipmentDefinition;

/** 角色出生时创建的一件装备 */
USTRUCT(BlueprintType)
struct FBBBDefaultEquipment
{
    GENERATED_BODY()

    /** 装备静态配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    /** 可选的快捷操作槽位 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "-1"))
    int32 QuickAccessSlot = INDEX_NONE;
};

/** 角色装备系统配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentConfig
{
    GENERATED_BODY()

    /** 右手装备挂接插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName RightHandWeaponSocketName = TEXT("WeaponGrip_R");

    /** 固定装备容器槽位数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 InventorySlotCount = 20;

    /** 快捷操作槽位数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 QuickAccessSlotCount = 5;

    /** 角色出生时创建的装备 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FBBBDefaultEquipment> DefaultEquipment;

    /** 是否默认装备第一件快捷装备 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bAutoEquipFirstQuickAccessEquipment = true;

    /** 网络装备句柄对应的静态配置表 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UBBBEquipmentCatalog> EquipmentCatalog = nullptr;
};
