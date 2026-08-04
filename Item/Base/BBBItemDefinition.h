#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBItemDefinition.generated.h"

class ABBBEquipmentActor;
class UAnimMontage;
class UBBBFireOperation;
class UBBBMagazineOperation;
class UTexture2D;

/** 物品静态配置与操作域插槽 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** 物品配置标识 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FName ItemId;

    /** 物品显示名称 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FText DisplayName;

    /** 物品描述 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FText Description;

    /** 物品图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    TObjectPtr<UTexture2D> Icon;

    /** 最大堆叠数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item", meta = (ClampMin = "1"))
    int32 MaxStack = 1;

    /** 装备实体类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TSubclassOf<ABBBEquipmentActor> EquipmentActorClass;

    /** 装备实体生成偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FTransform SpawnOffset = FTransform::Identity;

    /** 装备过渡动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Animation")
    TObjectPtr<UAnimMontage> EquipMontage = nullptr;

    /** 瞄准来源插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FName AimSourceSocketName = TEXT("Muzzle");

    /** 左手握持插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FName LeftHandGripSocketName = TEXT("LeftHand");

    /** 左手握持插槽修正 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FTransform LeftHandGripSocketLocalOffset = FTransform::Identity;

    /** 是否启用左手逆向动力学 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    bool bEnableLeftHandIK = true;

    /** 开火操作域 */
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment|Operation")
    TObjectPtr<UBBBFireOperation> FireOperation;

    /** 弹匣操作域 */
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment|Operation")
    TObjectPtr<UBBBMagazineOperation> MagazineOperation;
};
