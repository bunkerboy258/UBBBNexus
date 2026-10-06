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
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "右手武器插槽名"))
    FName RightHandWeaponSocketName = TEXT("WeaponGrip_R");

    /** 网络装备句柄对应的静态配置表 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备目录表"))
    TObjectPtr<UBBBEquipmentCatalog> EquipmentCatalog = nullptr;
};
