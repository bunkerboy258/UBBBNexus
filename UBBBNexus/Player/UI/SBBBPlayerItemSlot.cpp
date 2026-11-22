#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStage.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "Algo/Reverse.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInterface.h"
#include "Input/DragAndDrop.h"
#include "Rendering/DrawElementTypes.h"
#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
/** 一次拖动携带起始身份 不保存第二份物品容器 */
class FBBBPlayerItemDrag final : public FGameDragDropOperation
{
  public:
    DRAG_DROP_OPERATOR_TYPE(FBBBPlayerItemDrag, FGameDragDropOperation)
    TWeakObjectPtr<UBBBPlayerItemView> View;
    TWeakObjectPtr<APawn> Pawn;
    FGuid InstanceId;
    int32 Source = INDEX_NONE;

    static TSharedRef<FBBBPlayerItemDrag> New(UBBBPlayerItemView &Owner, int32 Slot,
        const FBBBPlayerItemDisplayData &Item, FVector2D CursorPosition)
    {
        TSharedRef<FBBBPlayerItemDrag> Operation = MakeShared<FBBBPlayerItemDrag>();
        Operation->View = &Owner;
        Operation->Pawn = Owner.GetItemController()->GetPawn().Get();
        Operation->InstanceId = Item.InstanceId;
        Operation->Source = Slot;
        Operation->DecoratorPosition = CursorPosition + CursorOffset;
        Operation->Thumbnail.Reset(Item.Icon);
        if (Item.Icon)
        {
            Operation->ThumbnailBrush.SetResourceObject(Item.Icon);
            Operation->ThumbnailBrush.DrawAs = ESlateBrushDrawType::Image;
            Operation->ThumbnailBrush.ImageSize = FVector2D(Item.Icon->GetSizeX(), Item.Icon->GetSizeY());
            Operation->Decorator = SNew(SBox)
                .Visibility(EVisibility::HitTestInvisible)
                .WidthOverride(104.0f)
                .HeightOverride(104.0f)
                [
                    SNew(SScaleBox)
                    .Stretch(EStretch::ScaleToFit)
                    [
                        SNew(SImage)
                        .Image(&Operation->ThumbnailBrush)
                    ]
                ];
        }
        else
        {
            Operation->Decorator = SNew(SBorder)
                .Visibility(EVisibility::HitTestInvisible)
                .BorderImage(BBBItemUI::Slot())
                .Padding(12.0f)
                [
                    SNew(STextBlock)
                    .Text(Item.Name)
                    .Font(BBBItemUI::Font(12))
                    .ColorAndOpacity(FLinearColor::White)
                ];
        }
        Operation->bCreateNewWindow = false;
        Operation->Construct();
        return Operation;
    }

    virtual TSharedPtr<SWidget> GetDefaultDecorator() const override
    {
        return Decorator;
    }

    virtual void OnDragged(const FDragDropEvent &Event) override
    {
        DecoratorPosition = FVector2D(Event.GetScreenSpacePosition()) + CursorOffset;
    }

  private:
    inline static const FVector2D CursorOffset = FVector2D(14.0, 16.0);
    TStrongObjectPtr<UTexture2D> Thumbnail;
    FSlateBrush ThumbnailBrush;
    TSharedPtr<SWidget> Decorator;
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
                                                     .ColorAndOpacity(FLinearColor::White)
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
    IconTexture.Reset(bOuterRight ? Data.DisplayImage : Data.Icon);
    IconBrush.SetResourceObject(bOuterRight && Data.DisplayMaterial
        ? static_cast<UObject *>(Data.DisplayMaterial) : IconTexture.Get());
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
        const float Highlight = bDropTarget ? 1.0f : HoverAmount;
        const auto* Stage = View.IsValid() ? View->GetItemStage() : nullptr;
        const float Glow = Stage ? Stage->GetSlotGlowOpacity() : 0.0f;
        const FVector2D Size = Geometry.GetLocalSize();
        static const FSlateColorBrush Solid(FLinearColor::White);
        const auto PaintRect = [&](int32 DrawLayer, FVector2D Extent, FVector2f Offset, FLinearColor Color)
        {
            FSlateDrawElement::MakeBox(Elements, DrawLayer,
                Geometry.ToPaintGeometry(Extent, FSlateLayoutTransform(Offset)), &Solid, ESlateDrawEffect::None, Color);
        };
        PaintRect(Layer++, Size, FVector2f::ZeroVector, FLinearColor(0.28f, 0.30f, 0.33f, bOccupied ? 0.27f : 0.20f));

        const float Width = Size.X * 0.36f;
        const float Left = bOuterRight ? Size.X - Width : 0.0f;
        const float RestStrength = bSelected ? 0.90f : bOccupied ? 0.54f : 0.23f;
        const float Strength = FMath::Lerp(RestStrength, 0.90f, Highlight);
        const auto Position = [this, Width](float Distance)
        {
            return FVector2f(bOuterRight ? Width - Distance : Distance, 0.0f);
        };
        TArray<FSlateGradientStop> Stops;
        Stops.Emplace(Position(0.0f), FLinearColor(0.88f, 0.31f, 0.041f, Strength));
        Stops.Emplace(Position(Width * 0.16f), FLinearColor(0.43f, 0.11f, 0.24f, Strength * 0.75f));
        Stops.Emplace(Position(Width * 0.47f), FLinearColor(0.20f, 0.07f, 0.34f, Strength * 0.42f));
        Stops.Emplace(Position(Width), FLinearColor(0.10f, 0.04f, 0.20f, 0.0f));
        if (bOuterRight)
        {
            Algo::Reverse(Stops);
        }
        FSlateDrawElement::MakeGradient(Elements, Layer,
            Geometry.ToPaintGeometry(FVector2D(Width, Size.Y), FSlateLayoutTransform(FVector2f(Left, 0))),
            MoveTemp(Stops), Orient_Vertical);

        constexpr float Border = 2.0f;
        constexpr float Strip = 3.8f;
        const float InnerHeight = Size.Y - 2.0f * Border;
        const float StripX = bOuterRight ? Size.X - Border - Strip : Border;
        PaintRect(++Layer, FVector2D(Strip, InnerHeight), FVector2f(StripX, Border),
            FLinearColor(1.0f, 0.42f, 0.10f, Strength));

        if (Glow > 0.0f)
        {
            for (int32 Ring = 2; Ring >= 1; --Ring)
            {
                const float Spread = static_cast<float>(Ring) * 1.5f;
                const FSlateRoundedBoxBrush Halo(FLinearColor::Transparent, 1.0f + Spread,
                    FLinearColor(1.0f, 1.0f, 1.0f, Glow / Ring), Border + 2.0f * Spread);
                FSlateDrawElement::MakeBox(Elements, ++Layer,
                    Geometry.ToPaintGeometry(Size + FVector2D(Spread * 2.0f), FSlateLayoutTransform(FVector2f(-Spread))),
                    &Halo, ESlateDrawEffect::None, FLinearColor::Transparent);
            }
        }
        const FLinearColor RestBorder = bSelected ? FLinearColor(0.93f, 0.76f, 0.58f, 0.91f)
            : FLinearColor(0.67f, 0.68f, 0.69f, 0.57f);
        const FSlateRoundedBoxBrush Frame(FLinearColor::Transparent, 1.0f,
            FMath::Lerp(RestBorder, FLinearColor(1.0f, 0.44f, 0.12f, 1.0f), Highlight), Border);
        FSlateDrawElement::MakeBox(Elements, ++Layer, Geometry.ToPaintGeometry(),
            &Frame, ESlateDrawEffect::None, FLinearColor::Transparent);
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

void SBBBPlayerItemSlot::OnMouseEnter(const FGeometry& Geometry, const FPointerEvent& Event)
{
    SCompoundWidget::OnMouseEnter(Geometry, Event);
    BeginHoverTransition();
}

void SBBBPlayerItemSlot::OnMouseLeave(const FPointerEvent& Event)
{
    SCompoundWidget::OnMouseLeave(Event);
    BeginHoverTransition();
}

void SBBBPlayerItemSlot::BeginHoverTransition()
{
    if (!bCompact && !HoverTimer.IsValid())
    {
        HoverTimer = RegisterActiveTimer(0.0f,
            FWidgetActiveTimerDelegate::CreateSP(this, &SBBBPlayerItemSlot::UpdateHover));
    }
    Invalidate(EInvalidateWidgetReason::Paint);
}

EActiveTimerReturnType SBBBPlayerItemSlot::UpdateHover(double CurrentTime, float DeltaTime)
{
    const float Target = IsHovered() ? 1.0f : 0.0f;
    const auto* Stage = View.IsValid() ? View->GetItemStage() : nullptr;
    const float Duration = Stage ? Stage->GetSlotHoverSeconds() : 0.0f;
    HoverAmount = Duration > 0.0f ? FMath::FInterpConstantTo(HoverAmount, Target, DeltaTime, 1.0f / Duration) : Target;
    Invalidate(EInvalidateWidgetReason::Paint);
    if (HoverAmount == Target)
    {
        HoverTimer.Reset();
        return EActiveTimerReturnType::Stop;
    }
    return EActiveTimerReturnType::Continue;
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
    if (!Controller || !View->IsBackpackOpen() || !InstanceId.IsValid())
    {
        return FReply::Unhandled();
    }
    const FBBBPlayerItemDisplayData Item = Controller->GetItemDisplayData(Slot);
    if (Item.InstanceId != InstanceId)
    {
        return FReply::Unhandled();
    }
    return FReply::Handled().BeginDragDrop(
        FBBBPlayerItemDrag::New(*View.Get(), Slot, Item, FVector2D(Event.GetScreenSpacePosition())));
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
    Invalidate(EInvalidateWidgetReason::Paint);
    View->MoveItem(Operation->Source, Slot, Operation->Pawn.Get(), Operation->InstanceId);
    return FReply::Handled();
}
