#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/Equip/BBBEquipmentEquipConfig.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/Fire/BBBEquipmentFireConfig.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/Reload/BBBEquipmentReloadConfig.h"
#include "Engine/DataAsset.h"
#include "BBBEquipmentDefinition.generated.h"

class UAnimInstance;
class USkeletalMesh;
class UTexture2D;

/** 装备实例引用的纯静态配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FName EquipmentId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Animation")
    TSubclassOf<UAnimInstance> CharacterAnimationLayerClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Animation")
    TSubclassOf<UAnimInstance> EquipmentAnimationClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Visual")
    TObjectPtr<USkeletalMesh> EquipmentMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    FBBBEquipmentEquipConfig EquipConfig;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire")
    FBBBEquipmentFireConfig FireConfig;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Reload")
    FBBBEquipmentReloadConfig ReloadConfig;
};
