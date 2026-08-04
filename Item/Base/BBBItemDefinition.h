#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBItemDefinition.generated.h"

class UBBBEquipmentOperation;
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

    /** 可装备操作域 */
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment|Operation")
    TObjectPtr<UBBBEquipmentOperation> EquipmentOperation;

    /** 开火操作域 */
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment|Operation")
    TObjectPtr<UBBBFireOperation> FireOperation;

    /** 弹匣操作域 */
    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Equipment|Operation")
    TObjectPtr<UBBBMagazineOperation> MagazineOperation;
};
