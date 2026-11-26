#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Config/Appearance/BBBCharacterAppearancePreset.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Config/BBBAppearanceResource.h"
#include "BBBCharacterAppearanceConfig.generated.h"

class UMaterialInterface;

/** 角色外观的基础静态配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceConfig
{
    GENERATED_BODY()

    /** 非物品资源标识与资源定义 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "基础外观资源"))
    TMap<FName, FBBBAppearanceResource> BaseResources;
    /** 按整身基础资源标识配置作者支持的部位配色 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "整套配色预设"))
    TMap<FName, FBBBCharacterAppearancePreset> MaterialPresets;
    /** 独立穿戴网格替换整身模型对应部位时 用于隐藏原材质槽的材质 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "整身替换部位隐藏材质"))
    TSoftObjectPtr<UMaterialInterface> ReplacedPartMaterial;
    /** 部件对应的默认基础资源 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认基础部件"))
    TMap<FName, FName> DefaultBaseParts;
    /** 显示部位与蓝图机械组件名称的对应关系 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "显示组件名称"))
    TMap<FName, FName> ComponentNames;
    /** 本地角色独占的模板缓冲标记 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "角色轮廓模板值", ClampMin = "1", ClampMax = "255"))
    int32 ReadabilityStencil = 251;
    /** 内侧轮廓的屏幕像素宽度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "内侧轮廓宽度", ClampMin = "0", ClampMax = "2"))
    float InnerOutlineWidth = 1.0f;
    /** 内侧轮廓混合强度 零值关闭 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "内侧轮廓强度", ClampMin = "0", ClampMax = "1"))
    float InnerOutlineOpacity = 0.10f;
    /** 默认整体污渍 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认污渍"))
    float Dirt = 0.0f;
    /** 默认整体磨损 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认磨损"))
    float Weathering = 0.0f;

};
