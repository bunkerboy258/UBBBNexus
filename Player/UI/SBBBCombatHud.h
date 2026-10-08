#pragma once
#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

class UBBBPlayerItemView;

/** 只绘制玩家公开结果的极简战斗覆层 */
class SBBBCombatHud final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBBBCombatHud) : _View(nullptr) {}
        SLATE_ARGUMENT(UBBBPlayerItemView *, View)
    SLATE_END_ARGS()

    /** @param Arguments	所属玩家界面 @return 无 */
    void Construct(const FArguments &Arguments);
    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
    virtual int32 OnPaint(const FPaintArgs &Args, const FGeometry &Geometry, const FSlateRect &CullingRect,
        FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &Style, bool bParentEnabled) const override;

private:
    /** 所属界面不拥有玩法状态 */
    TWeakObjectPtr<UBBBPlayerItemView> View;
};
