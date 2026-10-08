#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationStyle.h"
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
        TWeakObjectPtr<AActor> Item;
        int32 Source = INDEX_NONE;
        FText Name;

        static TSharedRef<FBBBPlayerItemDrag> New(UBBBPlayerItemView &Owner, int32 Slot, AActor &Actor, FText Label)
        {
            TSharedRef<FBBBPlayerItemDrag> Operation = MakeShared<FBBBPlayerItemDrag>();
            Operation->View = &Owner;
            Operation->Pawn = Owner.GetItemController()->GetPawn();
            Operation->Item = &Actor;
            Operation->Source = Slot;
            Operation->Name = MoveTemp(Label);
            Operation->bCreateNewWindow = false;
            Operation->Construct();
            return Operation;
        }

        virtual TSharedPtr<SWidget> GetDefaultDecorator() const override
        {
            static const FSlateColorBrush Background(FLinearColor(0.015f, 0.015f, 0.015f, 0.9f));
            return SNew(SBorder).BorderImage(&Background).Padding(12.0f)
                [SNew(STextBlock).Text(Name).Font(BBBCustomizationStyle::GetCustomizationFont(12))
                    .ColorAndOpacity(FLinearColor::White)];
        }
    };
}

void SBBBPlayerItemSlot::Construct(const FArguments &Arguments)
{
    View = Arguments._View;
    Slot = Arguments._Slot;
    bCompact = Arguments._Compact;
    ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    const FBBBPlayerItemDisplayData Data = Controller ? Controller->GetItemDisplayData(Slot) : FBBBPlayerItemDisplayData();
    bOccupied = Data.bOccupied;
    ItemName = Data.Name;
    IconTexture.Reset(Data.Icon);
    IconBrush.SetResourceObject(IconTexture.Get());
    IconBrush.ImageSize = IconTexture.IsValid()
        ? FVector2D(IconTexture->GetSizeX(), IconTexture->GetSizeY()) : FVector2D(128.0f, 64.0f);
    IconBrush.DrawAs = ESlateBrushDrawType::Image;

    if (bOccupied && !bCompact)
    {
        SetToolTip(SNew(SToolTip)
            [SNew(STextBlock).Text(ItemName).Font(BBBCustomizationStyle::GetCustomizationFont(12))
                .ColorAndOpacity(FLinearColor::White)]);
    }
    ChildSlot
    [
        SNew(SBox).MinDesiredWidth(bCompact ? 124.0f : 94.0f).HeightOverride(bCompact ? 72.0f : 74.0f)
        [
            SNew(SOverlay)
            + SOverlay::Slot().Padding(12.0f, 18.0f, 12.0f, 12.0f)
            [
                SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
                [SNew(SImage).Image(&IconBrush).ColorAndOpacity(FLinearColor::White)
                    .Visibility(IconTexture.IsValid() ? EVisibility::HitTestInvisible : EVisibility::Hidden)]
            ]
            + SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).Padding(8.0f, 5.0f)
            [
                SNew(STextBlock).Text(IsQuickSlot() ? FText::AsNumber(Slot + 1) : FText::GetEmpty())
                .Font(BBBCustomizationStyle::GetCustomizationFont(10))
                .ColorAndOpacity(FLinearColor(0.65f, 0.65f, 0.65f))
            ]
        ]
    ];
}

int32 SBBBPlayerItemSlot::OnPaint(const FPaintArgs &Args, const FGeometry &Geometry,
    const FSlateRect &CullingRect, FSlateWindowElementList &Elements, int32 Layer,
    const FWidgetStyle &Style, bool bParentEnabled) const
{
    const FVector2D Size = Geometry.GetLocalSize();
    const bool bSelected = IsSelected();
    const float Opacity = Style.GetColorAndOpacityTint().A;
    const FLinearColor Edge(1.0f, 1.0f, 1.0f, Opacity * (bSelected ? 0.95f : (IsHovered() ? 0.6f : 0.20f)));
    static const FSlateColorBrush Fill(FLinearColor::White);
    FSlateDrawElement::MakeBox(Elements, Layer, Geometry.ToPaintGeometry(), &Fill,
        ESlateDrawEffect::None, FLinearColor(0.015f, 0.015f, 0.015f, Opacity * (bCompact ? 0.55f : 0.30f)));
    const TArray<FVector2D> Outline = {{0.5, 0.5}, {Size.X - 0.5, 0.5},
        {Size.X - 0.5, Size.Y - 0.5}, {0.5, Size.Y - 0.5}, {0.5, 0.5}};
    FSlateDrawElement::MakeLines(Elements, Layer + 1, Geometry.ToPaintGeometry(), Outline,
        ESlateDrawEffect::None, Edge, true, bSelected ? 1.5f : 0.75f);
    return SCompoundWidget::OnPaint(Args, Geometry, CullingRect, Elements, Layer + 2, Style, bParentEnabled);
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
        return FReply::Handled().DetectDrag(AsShared(), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

FReply SBBBPlayerItemSlot::OnDragDetected(const FGeometry &Geometry, const FPointerEvent &Event)
{
    ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    const TArray<AActor *> Items = Controller ? Controller->GetBackpackItems() : TArray<AActor *>();
    if (!Controller || !View->IsBackpackOpen() || !Items.IsValidIndex(Slot) || !IsValid(Items[Slot]))
    {
        return FReply::Unhandled();
    }
    return FReply::Handled().BeginDragDrop(FBBBPlayerItemDrag::New(*View.Get(), Slot, *Items[Slot], ItemName));
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
    View->MoveItem(Operation->Source, Slot, Operation->Pawn.Get(), Operation->Item.Get());
    return FReply::Handled();
}
