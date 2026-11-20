#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "Engine/Texture2D.h"
#include "Algo/Reverse.h"
#include "Materials/MaterialInterface.h"
#include "Input/DragAndDrop.h"
#include "Rendering/DrawElementTypes.h"
#include "Brushes/SlateColorBrush.h"
#include "Styling/CoreStyle.h"
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
    bOuterRight = Arguments._OuterRight;
    bWearSlot = !Arguments._PositionLabel.IsEmpty();
    Refresh();

    ChildSlot[SNew(SBox)
                  .WidthOverride(Arguments._Width)
                  .HeightOverride(Arguments._Height)
                      [SNew(SOverlay) +
                       SOverlay::Slot().Padding(12.0f, 12.0f, 12.0f,
                                                bCompact || bWearSlot ? 25.0f : 12.0f)[SNew(SScaleBox)
                                                                                          .Stretch(EStretch::ScaleToFit)
                                                                                          .RenderTransformPivot(FVector2D(0.5, 0.5))
                                                                                          .RenderTransform_Lambda([this]()
                                                                                          {
                                                                                              const bool bLift = !bCompact && bOccupied && (IsHovered() || bDropTarget);
                                                                                              return FSlateRenderTransform(FVector2f(0.0f, bLift ? -8.0f : 0.0f));
                                                                                          })[SNew(SImage)
                                                     .Image(&IconBrush)
                                                     .ColorAndOpacity(FLinearColor(0.98f, 0.95f, 0.89f))
                                                     .Visibility_Lambda([this]()
                                                     {
                                                         return IconBrush.GetResourceObject() ? EVisibility::HitTestInvisible : EVisibility::Hidden;
                                                     })]] +
                       SOverlay::Slot()
                           .HAlign(HAlign_Center)
                           .VAlign(VAlign_Bottom)
                           .Padding(0.0f, 4.0f)[SNew(STextBlock)
                                                    .Text(Arguments._PositionLabel)
                                                    .Font(BBBItemUI::ArtFont(14))
                                                    .ColorAndOpacity(BBBItemUI::MutedText)] +
                       SOverlay::Slot()
                           .HAlign(bCompact ? HAlign_Center : HAlign_Left)
                           .VAlign(bCompact ? VAlign_Bottom : VAlign_Top)
                           .Padding(8.0f, 5.0f)[SNew(STextBlock)
                                                    .Text_Lambda([this]() { return QuickLabel; })
                                                    .Font(BBBItemUI::Font(bCompact ? 16 : 14))
                                                    .ColorAndOpacity_Lambda([this]()
                                                    {
                                                        return IsSelected() ? FLinearColor::White : BBBItemUI::MutedText;
                                                    })] +
                       SOverlay::Slot()
                           .HAlign(HAlign_Right)
                           .VAlign(VAlign_Top)
                           .Padding(8.0f)[SNew(STextBlock)
                                              .Text_Lambda([this]() { return QuantityLabel; })
                                              .Font(BBBItemUI::Font(18))
                                              .ColorAndOpacity(FLinearColor::White)] +
                       SOverlay::Slot()
                           .HAlign(HAlign_Right)
                           .VAlign(VAlign_Bottom)
                           .Padding(10.0f)[SNew(SBox).WidthOverride(28.0f).HeightOverride(
                               28.0f)[SNew(SImage)
                                          .Image(&PropertyBrush)
                                          .Visibility_Lambda([this]()
                                          {
                                              return PropertyTexture.IsValid() ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
                                          })]]]];
}

void SBBBPlayerItemSlot::Refresh()
{
    ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    const FBBBPlayerItemDisplayData Data =
        Controller ? Controller->GetItemDisplayData(Slot) : FBBBPlayerItemDisplayData();
    const bool bTooltipChanged = bOccupied != Data.bOccupied || !ItemName.EqualTo(Data.Name);
    bOccupied = Data.bOccupied;
    InstanceId = Data.InstanceId;
    PropertyTexture.Reset(Data.PropertyIcon);
    PropertyBrush.SetResourceObject(PropertyTexture.Get());
    PropertyBrush.DrawAs = ESlateBrushDrawType::Image;
    PropertyBrush.ImageSize = FVector2D(256.0f);
    ItemName = Data.Name;
    QuickLabel = Data.bQuick ? FText::AsNumber(Data.QuickIndex + 1) : FText::GetEmpty();
    QuantityLabel = Data.Quantity > 0 && Data.bShowQuantity ? FText::AsNumber(Data.Quantity) : FText::GetEmpty();
    IconTexture.Reset(!bCompact && Data.DisplayImage ? Data.DisplayImage : Data.Icon);
    IconBrush.SetResourceObject(!bCompact && Data.DisplayMaterial ? static_cast<UObject *>(Data.DisplayMaterial) : IconTexture.Get());
    IconBrush.ImageSize =
        IconTexture.IsValid() ? FVector2D(IconTexture->GetSizeX(), IconTexture->GetSizeY()) : FVector2D(128.0f, 64.0f);
    IconBrush.DrawAs = ESlateBrushDrawType::Image;

    if (bTooltipChanged && !bCompact)
    {
        SetToolTip(TSharedPtr<IToolTip>());
    }
    if (bTooltipChanged && bOccupied && !bCompact)
    {
        SetToolTip(SNew(SToolTip)
                       .BorderImage(BBBItemUI::Slot())
                       .TextMargin(FMargin(12.0f, 8.0f))
                           [SNew(STextBlock)
                                .Text(ItemName)
                                .Font(BBBItemUI::Font(16))
                                .ColorAndOpacity(FLinearColor::White)]);
    }
    Invalidate(EInvalidateWidgetReason::Paint | EInvalidateWidgetReason::Layout);
}

int32 SBBBPlayerItemSlot::OnPaint(const FPaintArgs &Args, const FGeometry &Geometry, const FSlateRect &CullingRect,
                                  FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &Style,
                                  bool bParentEnabled) const
{
    if (!bCompact)
    {
        const bool bSelected = (View.IsValid() && View->GetInspectedSlot() == Slot) || IsSelected();
        const bool bHover = IsHovered() || bDropTarget;
        const FVector2D Size = Geometry.GetLocalSize();
        const float Width = Size.X / 3.0f;
        const float Left = bOuterRight ? Size.X - Width : 0.0f;
        const float Strength = bDropTarget || bHover ? 1.0f : bSelected ? 0.89f : bOccupied ? 0.65f : 0.33f;
        const auto Position = [this, Width](float Distance)
        {
            return FVector2f(bOuterRight ? Width - Distance : Distance, 0.0f);
        };
        TArray<FSlateGradientStop> Stops;
        Stops.Emplace(Position(0.0f), FLinearColor(1.0f, 0.42f, 0.045f, Strength));
        Stops.Emplace(Position(2.5f), FLinearColor(1.0f, 0.25f, 0.035f, Strength));
        Stops.Emplace(Position(Width * 0.11f), FLinearColor(0.52f, 0.12f, 0.24f, Strength * 0.77f));
        Stops.Emplace(Position(Width * 0.39f), FLinearColor(0.25f, 0.075f, 0.46f, Strength * 0.56f));
        Stops.Emplace(Position(Width), FLinearColor(0.17f, 0.05f, 0.34f, 0.0f));
        if (bOuterRight)
        {
            Algo::Reverse(Stops);
        }
        FSlateDrawElement::MakeGradient(Elements, Layer,
            Geometry.ToPaintGeometry(FVector2D(Width, Size.Y), FSlateLayoutTransform(FVector2f(Left, 0))),
            MoveTemp(Stops), Orient_Vertical);
        const auto* Strip = BBBItemUI::Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Scrollbar_Handler.T_Scrollbar_Handler"));
        FSlateDrawElement::MakeBox(Elements, ++Layer,
            Geometry.ToPaintGeometry(FVector2D(2.0f, Size.Y), FSlateLayoutTransform(FVector2f(bOuterRight ? Size.X - 2.0f : 0.0f, 0))),
            Strip, ESlateDrawEffect::None, FLinearColor(1.0f, 0.93f, 0.8f, Strength));
    }
    return SCompoundWidget::OnPaint(Args, Geometry, CullingRect, Elements, Layer + 1, Style, bParentEnabled);
}

bool SBBBPlayerItemSlot::IsQuickSlot() const
{
    const ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    return Controller && Controller->GetItemDisplayData(Slot).bQuick;
}

bool SBBBPlayerItemSlot::IsSelected() const
{
    const ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    if (!Controller)
    {
        return false;
    }
    int32 Start = 0;
    int32 Count = 0;
    Controller->GetConsumableQuickRange(Start, Count);
    if (Slot >= Start && Slot < Start + Count)
    {
        return Slot - Start == Controller->GetSelectedConsumableSlot();
    }
    return Slot == Controller->GetSelectedItemSlot();
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

FReply SBBBPlayerItemSlot::OnMouseButtonDoubleClick(const FGeometry &Geometry, const FPointerEvent &Event)
{
    if (View.IsValid() && View->IsBackpackOpen() && bOccupied && Event.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        View->EquipItem(Slot, View->GetItemController()->GetPawn(), InstanceId);
        return FReply::Handled();
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
    const bool bAccept = Operation.IsValid() && Operation->View == View;
    if (bDropTarget != bAccept)
    {
        bDropTarget = bAccept;
        Invalidate(EInvalidateWidgetReason::Paint);
    }
    return bAccept ? FReply::Handled() : FReply::Unhandled();
}

void SBBBPlayerItemSlot::OnDragLeave(const FDragDropEvent &Event)
{
    bDropTarget = false;
    Invalidate(EInvalidateWidgetReason::Paint);
}

FReply SBBBPlayerItemSlot::OnDrop(const FGeometry &Geometry, const FDragDropEvent &Event)
{
    const TSharedPtr<FBBBPlayerItemDrag> Operation = Event.GetOperationAs<FBBBPlayerItemDrag>();
    if (!View.IsValid() || !Operation.IsValid() || Operation->View != View)
    {
        return FReply::Unhandled();
    }
    bDropTarget = false;
    View->MoveItem(Operation->Source, Slot, Operation->Pawn.Get(), Operation->InstanceId);
    return FReply::Handled();
}
