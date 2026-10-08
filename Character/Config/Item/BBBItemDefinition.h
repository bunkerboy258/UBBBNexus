#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBItemType.h"
#include "BBBItemDefinition.generated.h"
class UTexture2D;
class UMaterialInterface;

/** 物品系统拥有的共享静态定义 不依赖外观系统 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    /** 目录内唯一的型号标识 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品标识"))
    FName ItemId;
    /** 物品所属类别 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "物品类别"))
    EBBBItemType ItemType = EBBBItemType::Wearable;
    /** 物品显示名称 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "显示名称"))
    FText DisplayName;
    /** 物品说明 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "说明"))
    FText Description;
    /** 物品显示图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "图标"))
    TObjectPtr<UTexture2D> Icon = nullptr;
    /** 只包含物品自身的详情图片 不包含人物身体 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "详情图片"))
    TObjectPtr<UTexture2D> DisplayImage = nullptr;
    /** 国旗图集等需要独立 UV 坐标的详情显示资源 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "详情材质"))
    TObjectPtr<UMaterialInterface> DisplayMaterial = nullptr;
    /** 叠加在主图标右下角的属性图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "属性图标"))
    TObjectPtr<UTexture2D> PropertyIcon = nullptr;
    /** 允许放入的穿戴物品位置 装备为空 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "穿戴位置"))
    FName WearSlot;
};
