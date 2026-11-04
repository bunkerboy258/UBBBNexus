#pragma once
#include "CoreMinimal.h"
#include "BBBMaodieTestAppearanceConfig.generated.h"

/** 耄耋专属橙色补光配置 */
USTRUCT(BlueprintType)
struct FBBBMaodieTestAppearanceConfig final
{
    GENERATED_BODY()

    /** 只在有画面的客户端创建专用灯光 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋|外观", meta = (DisplayName = "启用专用补光"))
    bool bLighting = true;

    /** 柔和正面补光强度 单位坎德拉 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋|外观", meta = (DisplayName = "补光强度", ClampMin = "0", ToolTip = "正面柔光强度 单位坎德拉"))
    float FillIntensity = 36.0f;

    /** 侧后方橙色轮廓光强度 单位坎德拉 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋|外观", meta = (DisplayName = "轮廓光强度", ClampMin = "0", ToolTip = "侧后轮廓光强度 单位坎德拉"))
    float RimIntensity = 48.0f;

    /** 暖橙补光 保留表情和毛发层次 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋|外观", meta = (DisplayName = "补光颜色"))
    FLinearColor FillColor = FLinearColor(1.0f, 0.56f, 0.24f);

    /** 更浓的琥珀橙色侧后光 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋|外观", meta = (DisplayName = "轮廓光颜色"))
    FLinearColor RimColor = FLinearColor(1.0f, 0.24f, 0.025f);

    /** 使用共享相机材质的橙色可见内侧轮廓 模板值为250 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋|外观", meta = (DisplayName = "启用橙色内侧轮廓", ToolTip = "仅描绘可见身体内缘 被遮挡时不显示"))
    bool bInnerOutline = true;
};
