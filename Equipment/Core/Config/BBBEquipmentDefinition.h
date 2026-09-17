#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/Fragment/BBBEquipmentEquipFragment.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/Fragment/BBBEquipmentFireFragment.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/Fragment/BBBEquipmentReloadFragment.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"
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
    UBBBEquipmentDefinition();

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBEquipmentEquipFragment> EquipFragment;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fire", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBEquipmentFireFragment> FireFragment;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Reload", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBEquipmentReloadFragment> ReloadFragment;

};
