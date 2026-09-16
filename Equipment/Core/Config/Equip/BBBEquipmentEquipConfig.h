#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentEquipConfig.generated.h"

class UAnimMontage;

/** 装备挂接与人物装备表现的静态配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentEquipConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    FTransform SpawnOffset = FTransform::Identity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    TObjectPtr<UAnimMontage> EquipMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    FName LeftHandSocketName = TEXT("LeftHand");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    FVector LeftHandSocketOffset = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    FVector LeftHandIKOffset = FVector::ZeroVector;
};
