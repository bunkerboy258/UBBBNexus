#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Config/BBBAppearanceResource.h"
#include "BBBCharacterAppearanceConfig.generated.h"

/** 角色外观的基础静态配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceConfig
{
    GENERATED_BODY()

    /** 非物品资源标识与资源定义 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "基础外观资源"))
    TMap<FName, FBBBAppearanceResource> BaseResources;
    /** 部件对应的默认基础资源 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认基础部件"))
    TMap<FName, FName> DefaultBaseParts;
    /** 显示部位与蓝图机械组件名称的对应关系 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "显示组件名称"))
    TMap<FName, FName> ComponentNames;
    /** 仅本机控制角色启用可读性照明 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "启用本地角色补光"))
    bool bReadabilityLighting = true;
    /** 柔和补光强度 单位坎德拉 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "补光强度", ClampMin = "0"))
    float FillIntensity = 24.0f;
    /** 肩背轮廓灯强度 单位坎德拉 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "轮廓光强度", ClampMin = "0"))
    float RimIntensity = 18.0f;
    /** 补光颜色 保持接近中性 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "角色补光颜色"))
    FLinearColor ReadabilityLightColor = FLinearColor(1.0f, 0.97f, 0.93f);
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
