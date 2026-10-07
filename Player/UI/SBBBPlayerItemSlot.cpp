#include "BBBWork/UBBBNexus/Player/UI/SBBBPlayerItemSlot.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationStyle.h"
#include "Engine/Texture2D.h"
#include "Input/DragAndDrop.h"
#include "Rendering/DrawElementTypes.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SOverlay.h"

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
            return SNew(SBorder)
                .BorderImage(BBBCustomizationStyle::GetCardBrush(true))
                .Padding(16.0f)
                [
                    SNew(STextBlock).Text(Name)
                    .Font(BBBCustomizationStyle::GetCustomizationFont(12))
                    .ColorAndOpacity(BBBCustomizationStyle::Ink)
                ];
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
    ItemName = !Data.Name.IsEmpty() ? Data.Name : FText::FromString(bOccupied ? TEXT("物品") : TEXT("空槽位"));
    IconTexture.Reset(Data.Icon);
    IconBrush.SetResourceObject(IconTexture.Get());
    IconBrush.ImageSize = IconTexture.IsValid()
        ? FVector2D(IconTexture->GetSizeX(), IconTexture->GetSizeY()) : FVector2D(72.0f, 40.0f);
    IconBrush.DrawAs = ESlateBrushDrawType::Image;

    if (bCompact)
    {
        ChildSlot
        [
            SNew(SBox).WidthOverride(54.0f).HeightOverride(30.0f).HAlign(HAlign_Center).VAlign(VAlign_Center)
            [
                SNew(STextBlock).Text(FText::AsNumber(Slot + 1))
                .Font(BBBCustomizationStyle::GetCustomizationFont(12))
                .ColorAndOpacity_Lambda([this]()
                {
                    return IsSelected() ? FLinearColor::White
                        : (bOccupied ? BBBCustomizationStyle::Ink : FLinearColor(0.30f, 0.32f, 0.33f));
                })
            ]
        ];
        return;
    }

    TSharedRef<SWidget> Artwork = IconTexture.IsValid()
        ? StaticCastSharedRef<SWidget>(SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
            [SNew(SImage).Image(&IconBrush)])
        : BBBCustomizationStyle::MakeIcon(bOccupied ? TEXT("Item") : TEXT("Empty"),
            bOccupied ? 38.0f : 23.0f, bOccupied ? BBBCustomizationStyle::Ink : FLinearColor(0.15f, 0.17f, 0.18f));
    ChildSlot
    [
        SNew(SBox).MinDesiredWidth(180.0f).HeightOverride(132.0f)
        [
            SNew(SOverlay)
            + SOverlay::Slot().Padding(1.0f)
            [
                SNew(SImage).Image(View->GetCardSurfaceBrush())
                .ColorAndOpacity(FLinearColor(0.70f, 0.67f, 0.60f, bOccupied ? 0.50f : 0.20f))
                .Visibility(EVisibility::HitTestInvisible)
            ]
            + SOverlay::Slot().Padding(12.0f, 8.0f)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight()
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot().AutoWidth()
                    [
                        SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("%02d"), Slot + 1)))
                        .Font(BBBCustomizationStyle::GetCustomizationFont(10))
                        .ColorAndOpacity(BBBCustomizationStyle::MutedInk)
                    ]
                    + SHorizontalBox::Slot().FillWidth(1.0f).HAlign(HAlign_Right)
                    [
                        SNew(STextBlock).Text_Lambda([this]() { return GetSlotStatus(); })
                        .Font(BBBCustomizationStyle::GetCustomizationFont(10))
                        .ColorAndOpacity(BBBCustomizationStyle::Accent)
                    ]
                ]
                + SVerticalBox::Slot().FillHeight(1.0f).Padding(10.0f, 5.0f)
                [Artwork]
                + SVerticalBox::Slot().AutoHeight()
                [
                    SNew(SBox).HeightOverride(38.0f).VAlign(VAlign_Center)
                    [
                        SNew(STextBlock).Text(ItemName)
                        .Font(BBBCustomizationStyle::GetCustomizationFont(11))
                        .ColorAndOpacity(bOccupied ? BBBCustomizationStyle::Ink : BBBCustomizationStyle::MutedInk)
                        .AutoWrapText(true).OverflowPolicy(ETextOverflowPolicy::Ellipsis)
                        .Justification(ETextJustify::Center)
                    ]
                ]
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
    const FLinearColor Accent = bCompact ? FLinearColor(1.0f, 0.27f, 0.025f) : BBBCustomizationStyle::Accent;
    const FLinearColor Edge = bSelected ? Accent : (IsHovered() && !bCompact
        ? BBBCustomizationStyle::Ink : FLinearColor(0.20f, 0.22f, 0.22f, bOccupied ? 0.80f : 0.35f));
    static const FSlateColorBrush Fill(FLinearColor::White);
    FSlateDrawElement::MakeBox(Elements, Layer, Geometry.ToPaintGeometry(), &Fill,
        ESlateDrawEffect::None, bSelected ? FLinearColor(0.10f, 0.046f, 0.014f, 0.80f)
            : FLinearColor(0.004f, 0.006f, 0.010f, bCompact ? 0.72f : 0.46f));
    const float Cut = bCompact ? 4.0f : 9.0f;
    TArray<FVector2D> Outline = {{0.5, 0.5}, {Size.X - Cut, 0.5}, {Size.X - 0.5, Cut},
        {Size.X - 0.5, Size.Y - 0.5}, {Cut, Size.Y - 0.5}, {0.5, Size.Y - Cut}, {0.5, 0.5}};
    FSlateDrawElement::MakeLines(Elements, Layer + 1, Geometry.ToPaintGeometry(), Outline,
        ESlateDrawEffect::None, Edge, true, bSelected ? 1.5f : 0.7f);
    if (bOccupied)
    {
        TArray<FVector2D> Marker = {{Cut + 1.0, Size.Y - 1.5}, {Size.X - 1.5, Size.Y - 1.5}};
        FSlateDrawElement::MakeLines(Elements, Layer + 1, Geometry.ToPaintGeometry(), Marker,
            ESlateDrawEffect::None, bSelected ? Accent : Edge, true, bSelected ? 2.5f : 1.0f);
    }
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

FText SBBBPlayerItemSlot::GetSlotStatus() const
{
    const ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    const FBBBPlayerItemDisplayData Data = Controller ? Controller->GetItemDisplayData(Slot) : FBBBPlayerItemDisplayData();
    if (Data.bActive)
    {
        return FText::FromString(TEXT("手持"));
    }
    if (Data.bSelected)
    {
        return FText::FromString(TEXT("已选择"));
    }
    return Data.bQuick ? FText::FromString(TEXT("快捷")) : FText::GetEmpty();
}

void SBBBPlayerItemSlot::OnMouseEnter(const FGeometry &Geometry, const FPointerEvent &Event)
{
    SCompoundWidget::OnMouseEnter(Geometry, Event);
    if (View.IsValid())
    {
        View->InspectSlot(Slot);
    }
}

FReply SBBBPlayerItemSlot::OnMouseButtonDown(const FGeometry &Geometry, const FPointerEvent &Event)
{
    if (View.IsValid() && Event.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        View->InspectSlot(Slot);
        if (View->IsBackpackOpen() && bOccupied)
        {
            return FReply::Handled().DetectDrag(AsShared(), EKeys::LeftMouseButton);
        }
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

FReply SBBBPlayerItemSlot::OnMouseButtonUp(const FGeometry &Geometry, const FPointerEvent &Event)
{
    if (!View.IsValid() || !IsQuickSlot())
    {
        return FReply::Unhandled();
    }
    if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        View->SelectSlot(Slot);
        return FReply::Handled();
    }
    if (Event.GetEffectingButton() == EKeys::RightMouseButton)
    {
        View->SelectSlot(INDEX_NONE);
        return FReply::Handled();
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
