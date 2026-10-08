#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "Engine/Texture2D.h"
#include "Input/DragAndDrop.h"
#include "Rendering/DrawElementTypes.h"
#include "Brushes/SlateColorBrush.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SToolTip.h"

namespace
{
/** 一次拖动携带起始身份 不保存第二份物品容器 */
class FBBBPlayerItemDrag final : public FDragDropOperation
{
  public:
    DRAG_DROP_OPERATOR_TYPE(FBBBPlayerItemDrag, FDragDropOperation)
    TWeakObjectPtr<UBBBPlayerItemView> View;
    TWeakObjectPtr<APawn> Pawn;
    FGuid InstanceId;
    int32 Source = INDEX_NONE;
    FText Name;

    static TSharedRef<FBBBPlayerItemDrag> New(UBBBPlayerItemView &Owner, int32 Slot, FGuid Identity, FText Label)
    {
        TSharedRef<FBBBPlayerItemDrag> Operation = MakeShared<FBBBPlayerItemDrag>();
        Operation->View = &Owner;
        Operation->Pawn = Owner.GetItemController()->GetPawn();
        Operation->InstanceId = Identity;
        Operation->Source = Slot;
        Operation->Name = MoveTemp(Label);
        Operation->bCreateNewWindow = false;
        Operation->Construct();
        return Operation;
    }

    virtual TSharedPtr<SWidget> GetDefaultDecorator() const override
    {
        return SNew(SBorder)
            .BorderImage(BBBItemUI::Slot())
            .Padding(12.0f)[SNew(STextBlock).Text(Name).Font(BBBItemUI::Font(12)).ColorAndOpacity(FLinearColor::White)];
    }
};
}

void SBBBPlayerItemSlot::Construct(const FArguments &Arguments)
{
    View = Arguments._View;
    Slot = Arguments._Slot;
    bCompact = Arguments._Compact;
    bWearSlot = !Arguments._PositionLabel.IsEmpty();
    ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    const FBBBPlayerItemDisplayData Data =
        Controller ? Controller->GetItemDisplayData(Slot) : FBBBPlayerItemDisplayData();
    bOccupied = Data.bOccupied;
    InstanceId = Data.InstanceId;
    PropertyTexture.Reset(Data.PropertyIcon);
    PropertyBrush.SetResourceObject(PropertyTexture.Get());
    PropertyBrush.DrawAs = ESlateBrushDrawType::Image;
    PropertyBrush.ImageSize = FVector2D(256.0f);
    ItemName = Data.Name;
    IconTexture.Reset(Data.Icon);
    IconBrush.SetResourceObject(IconTexture.Get());
    IconBrush.ImageSize =
        IconTexture.IsValid() ? FVector2D(IconTexture->GetSizeX(), IconTexture->GetSizeY()) : FVector2D(128.0f, 64.0f);
    IconBrush.DrawAs = ESlateBrushDrawType::Image;

    if (bOccupied && !bCompact)
    {
        SetToolTip(SNew(SToolTip)
                       .BorderImage(BBBItemUI::Slot())
                       .TextMargin(FMargin(12.0f, 8.0f))
                           [SNew(STextBlock)
                                .Text(ItemName)
                                .Font(BBBItemUI::Font(16))
                                .ColorAndOpacity(FLinearColor::White)]);
    }
    ChildSlot[SNew(SBox)
                  .WidthOverride(Arguments._Width)
                  .HeightOverride(Arguments._Height)
                      [SNew(SOverlay) +
                       SOverlay::Slot().Padding(12.0f, 12.0f, 12.0f,
                                                bCompact || bWearSlot ? 25.0f : 12.0f)[SNew(SScaleBox).Stretch(
                           EStretch::ScaleToFit)[SNew(SImage)
                                                     .Image(&IconBrush)
                                                     .ColorAndOpacity(FLinearColor(0.82f, 0.82f, 0.79f))
                                                     .Visibility(IconTexture.IsValid() ? EVisibility::HitTestInvisible
                                                                                       : EVisibility::Hidden)]] +
                       SOverlay::Slot()
                           .HAlign(HAlign_Center)
                           .VAlign(VAlign_Bottom)
                           .Padding(0.0f, 4.0f)[SNew(STextBlock)
                                                    .Text(Arguments._PositionLabel)
                                                    .Font(BBBItemUI::Font(14))
                                                    .ColorAndOpacity(FLinearColor(0.68f, 0.68f, 0.64f))] +
                       SOverlay::Slot()
                           .HAlign(bCompact ? HAlign_Center : HAlign_Left)
                           .VAlign(bCompact ? VAlign_Bottom : VAlign_Top)
                           .Padding(8.0f, 5.0f)[SNew(STextBlock)
                                                    .Text(IsQuickSlot() ? FText::AsNumber(Slot + 1) : FText::GetEmpty())
                                                    .Font(BBBItemUI::Font(bCompact ? 16 : 14))
                                                    .ColorAndOpacity_Lambda(
                                                        [this]()
                                                        {
                                                            return IsSelected() ? BBBItemUI::Accent
                                                                                : FLinearColor(0.8f, 0.8f, 0.8f);
                                                        })] +
                       SOverlay::Slot()
                           .HAlign(HAlign_Right)
                           .VAlign(VAlign_Bottom)
                           .Padding(10.0f)[SNew(SBox).WidthOverride(28.0f).HeightOverride(
                               28.0f)[SNew(SImage)
                                          .Image(&PropertyBrush)
                                          .Visibility(PropertyTexture.IsValid() ? EVisibility::HitTestInvisible
                                                                                : EVisibility::Collapsed)]]]];
}

int32 SBBBPlayerItemSlot::OnPaint(const FPaintArgs &Args, const FGeometry &Geometry, const FSlateRect &CullingRect,
                                  FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &Style,
                                  bool bParentEnabled) const
{
    if (!bCompact)
    {
        const bool bSelected = View.IsValid() && View->GetInspectedSlot() == Slot;
        const FLinearColor Tint = bSelected ? FLinearColor(1.0f, 0.72f, 0.38f, 0.94f)
                                          : FLinearColor(1.0f, 1.0f, 0.98f, IsHovered() ? 1.0f : 0.85f);
        FSlateDrawElement::MakeBox(Elements, Layer, Geometry.ToPaintGeometry(),
                                   BBBItemUI::Slot(), ESlateDrawEffect::None,
                                   Tint * Style.GetColorAndOpacityTint());
        if (bSelected || (bOccupied && IsHovered()))
        {
            const FLinearColor Edge = bSelected ? BBBItemUI::Accent : BBBItemUI::MutedText * 0.55f;
            FSlateDrawElement::MakeBox(Elements, ++Layer, Geometry.ToPaintGeometry(), BBBItemUI::Selection(),
                                       ESlateDrawEffect::None, Edge * Style.GetColorAndOpacityTint());
        }
    }
    return SCompoundWidget::OnPaint(Args, Geometry, CullingRect, Elements, Layer + 1, Style, bParentEnabled);
}

bool SBBBPlayerItemSlot::IsQuickSlot() const
{
    const ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    return Controller && Slot >= 0 && Slot < Controller->GetQuickAccessSlotCount();
}

bool SBBBPlayerItemSlot::IsSelected() const
{
    const ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    return Controller && Slot == Controller->GetSelectedItemSlot();
}

FReply SBBBPlayerItemSlot::OnMouseButtonDown(const FGeometry &Geometry, const FPointerEvent &Event)
{
    if (View.IsValid() && View->IsBackpackOpen() && bOccupied && Event.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        View->InspectSlot(Slot);
        return FReply::Handled().DetectDrag(AsShared(), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

FReply SBBBPlayerItemSlot::OnDragDetected(const FGeometry &Geometry, const FPointerEvent &Event)
{
    ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    if (!Controller || !View->IsBackpackOpen() || !InstanceId.IsValid() ||
        Controller->GetItemDisplayData(Slot).InstanceId != InstanceId)
    {
        return FReply::Unhandled();
    }
    return FReply::Handled().BeginDragDrop(FBBBPlayerItemDrag::New(*View.Get(), Slot, InstanceId, ItemName));
}

FReply SBBBPlayerItemSlot::OnDragOver(const FGeometry &Geometry, const FDragDropEvent &Event)
{
    const TSharedPtr<FBBBPlayerItemDrag> Operation = Event.GetOperationAs<FBBBPlayerItemDrag>();
    return Operation.IsValid() && Operation->View == View ? FReply::Handled() : FReply::Unhandled();
}

FReply SBBBPlayerItemSlot::OnDrop(const FGeometry &Geometry, const FDragDropEvent &Event)
{
    const TSharedPtr<FBBBPlayerItemDrag> Operation = Event.GetOperationAs<FBBBPlayerItemDrag>();
    if (!View.IsValid() || !Operation.IsValid() || Operation->View != View)
    {
        return FReply::Unhandled();
    }
    View->MoveItem(Operation->Source, Slot, Operation->Pawn.Get(), Operation->InstanceId);
    return FReply::Handled();
}
