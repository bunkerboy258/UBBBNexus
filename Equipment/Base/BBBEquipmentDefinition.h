#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBEquipmentDefinition.generated.h"

class UBBBEquipFragment;
class UBBBSingleProjectileFireFragment;
class UBBBMagazineFragment;
class UTexture2D;

/** 装备静态配置与领域插槽 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** 装备配置标识 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FName EquipmentId;

    /** 装备显示名称 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FText DisplayName;

    /** 装备说明 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FText Description;

    /** 装备图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TObjectPtr<UTexture2D> Icon;

    /** 装备领域 */
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment|Fragment")
    TObjectPtr<UBBBEquipFragment> EquipFragment;

    /** 开火领域 */
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment|Fragment")
    TObjectPtr<UBBBSingleProjectileFireFragment> FireFragment;

    /** 弹匣领域 */
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment|Fragment")
    TObjectPtr<UBBBMagazineFragment> MagazineFragment;
};
