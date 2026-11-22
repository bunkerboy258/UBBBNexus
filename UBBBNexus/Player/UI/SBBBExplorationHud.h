#pragma once
#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

class UBBBPlayerItemView;

/** 探索完成提示与教学覆层 */
class SBBBExplorationHud final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBBBExplorationHud) : _View(nullptr) {}
        SLATE_ARGUMENT(UBBBPlayerItemView *, View)
    SLATE_END_ARGS()

    /** @param Arguments\t所属界面 @return 无 */
    void Construct(const FArguments &Arguments);
    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
    virtual int32 OnPaint(const FPaintArgs &Args, const FGeometry &Geometry, const FSlateRect &CullingRect,
        FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &Style, bool bParentEnabled) const override;

private:
    /** 不拥有探索玩法状态 */
    TWeakObjectPtr<UBBBPlayerItemView> View;
};
