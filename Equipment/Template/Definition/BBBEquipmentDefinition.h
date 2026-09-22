#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBEquipmentDefinition.generated.h"

class ABBBEquipment;
class UBBBEquipmentAnimInstance;
class UAnimInstance;
class USkeletalMesh;
class UTexture2D;

/** 所有装备实例共享的静态资源配置 */
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
    TSubclassOf<UBBBEquipmentAnimInstance> EquipmentAnimationClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Visual")
    TObjectPtr<USkeletalMesh> EquipmentMesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Visual")
    FTransform SpawnOffset = FTransform::Identity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TSubclassOf<ABBBEquipment> EquipmentClass;
};
