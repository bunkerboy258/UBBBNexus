#pragma once
#include "CoreMinimal.h"
#include "BBBEquipmentConfig.generated.h"

USTRUCT(BlueprintType)
//定义装备挂接与瞄准骨骼名称
struct FBBBCharacterEquipmentConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName RightHandWeaponSocketName = TEXT("WeaponGrip_R");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AimSourceBoneName = TEXT("hand_r");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName LeftHandIKBoneName = TEXT("hand_l");
};
