#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBFireDomin.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineDomin.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"
#include "BBBEquipmentDefinition.generated.h"

class UAnimSequence;
class UTexture2D;

/** 装备提供给角色的持续上半身动画配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentUpperBodyAnimationConfig
{
    GENERATED_BODY()

    /** 普通持有装备时使用的上半身动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Animation")
    TObjectPtr<UAnimSequence> HoldingAnimation = nullptr;

    /** 瞄准时使用的上半身动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Animation")
    TObjectPtr<UAnimSequence> AimingAnimation = nullptr;
};

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

    /** 装备提供给角色的持续上半身动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Animation")
    FBBBEquipmentUpperBodyAnimationConfig UpperBodyAnimation;

    /** 装备领域 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fragment", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBEquipDomin> EquipDomin;

    /** 开火领域 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fragment", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBFireDomin> FireDomin;

    /** 弹匣领域 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Fragment", meta = (ExcludeBaseStruct))
    TInstancedStruct<FBBBMagazineDomin> MagazineDomin;
};
