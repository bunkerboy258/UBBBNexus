#pragma once
#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

class UBBBPlayerItemView;

/** 探索计数与完成提示 只读取控制器结果 */
class SBBBExplorationHud final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBBBExplorationHud) : _View(nullptr) {}
        SLATE_ARGUMENT(UBBBPlayerItemView *, View)
    SLATE_END_ARGS()

    /** @param Arguments\t所属界面 @return 无 */
    void Construct(const FArguments &Arguments);
    virtual void Tick(const FGeometry &Geometry, double CurrentTime, float DeltaTime) override;
    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
    virtual int32 OnPaint(const FPaintArgs &Args, const FGeometry &Geometry, const FSlateRect &CullingRect,
        FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &Style, bool bParentEnabled) const override;

private:
    /** 不拥有探索玩法状态 */
    TWeakObjectPtr<UBBBPlayerItemView> View;
    /** 只用于加一时的短暂显示反馈 */
    int32 PreviousCount = 0;
    float PulseAge = 1.0f;
};
