#pragma once

#include "Widgets/SLeafWidget.h"

/** 换装部位与操作的矢量图标 */
class SBBBCharacterCustomizationIcon final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBBBCharacterCustomizationIcon)
        : _Symbol(NAME_None)
        , _Size(24.0f)
        , _Color(FLinearColor(0.78f, 0.81f, 0.82f))
    {
    }
        SLATE_ARGUMENT(FName, Symbol)
        SLATE_ARGUMENT(float, Size)
        SLATE_ATTRIBUTE(FLinearColor, Color)
    SLATE_END_ARGS()

    /**
     * 配置图标内容
     * @param InArgs\t图标参数
     * @return 无
     */
    void Construct(const FArguments& InArgs);

protected:
    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& Geometry,
        const FSlateRect& CullingRect, FSlateWindowElementList& Elements, int32 Layer,
        const FWidgetStyle& Style, bool bParentEnabled) const override;

private:
    /** 当前绘制的部位或操作符号 */
    FName Symbol;
    /** 图标的最大逻辑边长 */
    float Size = 24.0f;
    /** 随悬停或选中状态更新的颜色 */
    TAttribute<FLinearColor> Color;
};
