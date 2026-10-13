#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Item/BBBItemType.h"
#include "BBBItemDefinition.generated.h"
class UTexture2D;
class UMaterialInterface;

/** 物品系统拥有的共享静态定义 不依赖外观系统 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    /** 同一格子允许保存的最大数量 装备与穿戴物固定为一 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|存储", meta = (DisplayName = "最大堆叠", ClampMin = "1", ClampMax = "1000", ToolTip = "同型号消耗品和杂物允许合并 装备与穿戴物必须为一"))
    int32 MaxStack = 1;
    /** 目录内唯一的型号标识 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|标识", meta = (DisplayName = "型号标识", ToolTip = "目录内唯一的型号名称 用于物品检索和同步 不是单件物品实例标识"))
    FName ItemId;
    /** 物品所属类别 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|标识", meta = (DisplayName = "物品类别", ToolTip = "区分装备与穿戴物品 武器应为装备 不代表步枪或手枪等具体类型"))
    EBBBItemType ItemType = EBBBItemType::Wearable;
    /** 物品显示名称 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|显示", meta = (DisplayName = "显示名称", ToolTip = "背包和物品详情中显示的本地化名称 不参与型号检索"))
    FText DisplayName;
    /** 物品说明 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|显示", meta = (DisplayName = "详情说明", ToolTip = "物品详情界面的说明文字 不决定玩法"))
    FText Description;
    /** 物品显示图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|显示", meta = (DisplayName = "格子图标", ToolTip = "背包和快捷格显示的主图标 详情图片为空时也作为详情图片"))
    TObjectPtr<UTexture2D> Icon = nullptr;
    /** 只包含物品自身的详情图片 不包含人物身体 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|显示", meta = (DisplayName = "详情图片", ToolTip = "仅显示物品自身的详情图 为空时使用格子图标 人物预览不使用此图"))
    TObjectPtr<UTexture2D> DisplayImage = nullptr;
    /** 国旗图集等需要独立 UV 坐标的详情显示资源 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|显示", meta = (DisplayName = "详情材质", ToolTip = "详情图优先使用此材质 为空时使用详情图片或格子图标 用于独立 UV 等显示需求"))
    TObjectPtr<UMaterialInterface> DisplayMaterial = nullptr;
    /** 叠加在主图标右下角的属性图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|显示", meta = (DisplayName = "属性角标", ToolTip = "叠加在物品格子主图标上的属性标记 不替代主图标"))
    TObjectPtr<UTexture2D> PropertyIcon = nullptr;
    /** 允许放入的穿戴物品位置 装备为空 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|物品|穿戴", meta = (DisplayName = "穿戴位置", ToolTip = "穿戴物品允许放入的位置名 武器等装备必须为空 不用于主手挂接插槽"))
    FName WearSlot;
};
