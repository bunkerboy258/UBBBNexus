#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/SlateBrush.h"
#include "UObject/StrongObjectPtr.h"

class UBBBPlayerItemView;
class UTexture2D;

/** 背包索引对应的单个显示与拖动控件 */
class SBBBPlayerItemSlot final : public SCompoundWidget
{
  public:
    SLATE_BEGIN_ARGS(SBBBPlayerItemSlot)
        : _View(nullptr), _Slot(INDEX_NONE), _Compact(false), _Width(154.0f), _Height(102.0f)
    {
    }
    SLATE_ARGUMENT(UBBBPlayerItemView *, View)
    SLATE_ARGUMENT(int32, Slot)
    SLATE_ARGUMENT(bool, Compact)
    SLATE_ARGUMENT(float, Width)
    SLATE_ARGUMENT(float, Height)
    SLATE_ARGUMENT(FText, PositionLabel)
    SLATE_END_ARGS()

    /** @param Arguments	界面和槽位参数 @return 无 */
    void Construct(const FArguments &Arguments);

    virtual FReply OnMouseButtonDown(const FGeometry &Geometry, const FPointerEvent &Event) override;
    virtual FReply OnMouseButtonDoubleClick(const FGeometry &Geometry, const FPointerEvent &Event) override;
    virtual FReply OnDragDetected(const FGeometry &Geometry, const FPointerEvent &Event) override;
    virtual FReply OnDragOver(const FGeometry &Geometry, const FDragDropEvent &Event) override;
    virtual FReply OnDrop(const FGeometry &Geometry, const FDragDropEvent &Event) override;
    virtual int32 OnPaint(const FPaintArgs &Args, const FGeometry &Geometry, const FSlateRect &CullingRect,
                          FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &Style,
                          bool bParentEnabled) const override;

  private:
    /** @return 当前控件是否对应快捷槽位 */
    bool IsQuickSlot() const;

    /** @return 当前快捷格子是否被选中 */
    bool IsSelected() const;

    TWeakObjectPtr<UBBBPlayerItemView> View;
    TStrongObjectPtr<UTexture2D> IconTexture;
    FSlateBrush IconBrush;
    TStrongObjectPtr<UTexture2D> PropertyTexture;
    FSlateBrush PropertyBrush;
    FGuid InstanceId;
    FText ItemName;
    int32 Slot = INDEX_NONE;
    bool bOccupied = false;
    bool bCompact = false;
    bool bWearSlot = false;
};
