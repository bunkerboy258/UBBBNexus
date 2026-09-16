#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentConfig.generated.h"

class UBBBEquipmentCatalog;

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

    /** 网络装备句柄对应的静态配置表 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UBBBEquipmentCatalog> EquipmentCatalog = nullptr;
};
