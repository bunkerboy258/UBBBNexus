#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentConfig.generated.h"


/** 角色装备系统配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentConfig
{
    GENERATED_BODY()

    /** 右手装备挂接插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "右手武器插槽名"))
    FName RightHandWeaponSocketName = TEXT("WeaponGrip_R");

};
