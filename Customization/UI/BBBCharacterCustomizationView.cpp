#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationView.h"
#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Text/STextBlock.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBCustomizationView, Log, All);

namespace
{
FText GetPartSlotTitle(const FName PartSlot)
{
    if (PartSlot == TEXT("Body"))
    {
        return FText::FromString(TEXT("上衣"));
    }
    if (PartSlot == TEXT("Head"))
    {
        return FText::FromString(TEXT("头部"));
    }
    if (PartSlot == TEXT("Vest"))
    {
        return FText::FromString(TEXT("战术背心"));
    }
    if (PartSlot == TEXT("Arms"))
    {
        return FText::FromString(TEXT("手臂"));
    }
    if (PartSlot == TEXT("Helmet"))
    {
        return FText::FromString(TEXT("头盔"));
    }
    if (PartSlot == TEXT("Legs"))
    {
        return FText::FromString(TEXT("裤装"));
    }
    if (PartSlot == TEXT("Boots"))
    {
        return FText::FromString(TEXT("靴子"));
    }
    if (PartSlot == TEXT("Backpack"))
    {
        return FText::FromString(TEXT("背包"));
    }
    if (PartSlot == TEXT("Belt"))
    {
        return FText::FromString(TEXT("腰带"));
    }
    if (PartSlot == TEXT("Attachments"))
    {
        return FText::FromString(TEXT("挂载组合"));
    }

    return FText::FromName(PartSlot);
}

TArray<FName> GetPartSlotOrder(const bool bLeft)
{
    if (bLeft)
    {
        return {TEXT("Head"), TEXT("Helmet"), TEXT("Body"), TEXT("Arms"), TEXT("Vest")};
    }

    return {TEXT("Backpack"), TEXT("Belt"), TEXT("Legs"), TEXT("Boots"), TEXT("Attachments")};
}

FText GetViewTitle(const FName ViewName)
{
    if (ViewName == TEXT("Full"))
    {
        return FText::FromString(TEXT("全身"));
    }
    if (ViewName == TEXT("Head"))
    {
        return FText::FromString(TEXT("头部"));
    }

    return FText::FromString(TEXT("腿部"));
}

FSlateFontInfo GetCustomizationFont(const int32 Size)
{
    FSlateFontInfo Font = FCoreStyle::Get().GetFontStyle("NormalFont");
    Font.Size = Size;
    return Font;
}

const FTextBlockStyle &GetCustomizationButtonTextStyle()
{
    static const FTextBlockStyle Style = []()
    {
        FTextBlockStyle Result = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("ButtonText");
        Result.SetFont(GetCustomizationFont(18));
        return Result;
    }();
    return Style;
}
}

UBBBCharacterCustomizationView::UBBBCharacterCustomizationView(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsFocusable(true);
}

void UBBBCharacterCustomizationView::SetSession(UBBBCharacterCustomizationSession *InSession)
{
    Session = InSession;
}

void UBBBCharacterCustomizationView::SetPreviewTexture(UTextureRenderTarget2D *InTexture)
{
    PreviewTexture = InTexture;
    PreviewBrush.SetResourceObject(InTexture);
    if (InTexture)
    {
        PreviewBrush.ImageSize = FVector2D(InTexture->SizeX, InTexture->SizeY);
    }
}

FName UBBBCharacterCustomizationView::GetSelectedItemId(const FName PartSlot) const
{
    if (!Session)
    {
        return NAME_None;
    }

    const FBBBAppearanceSelection Selection = Session->GetDraft();
    if (PartSlot == TEXT("Attachments"))
    {
        return Selection.Attachments;
    }

    for (const FBBBAppearancePart &Part : Selection.Parts)
    {
        if (Part.Slot == PartSlot)
        {
            return Part.Item;
        }
    }

    return NAME_None;
}

FText UBBBCharacterCustomizationView::GetSelectedItem(const FName PartSlot) const
{
    return GetItemDisplayName(GetSelectedItemId(PartSlot));
}

FText UBBBCharacterCustomizationView::GetItemDisplayName(const FName ItemId) const
{
    if (ItemId.IsNone())
    {
        return FText::FromString(TEXT("未选择"));
    }

    FBBBAppearanceItem Item;
    if (!Session || !Session->GetItem(ItemId, Item))
    {
        return FText::FromString(TEXT("配置不可用"));
    }
    if (Item.DisplayName.IsEmpty())
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("外观款式缺少中文显示名 Item=%s"), *ItemId.ToString());
        return FText::FromString(TEXT("未命名款式"));
    }

    return Item.DisplayName;
}

int32 UBBBCharacterCustomizationView::GetSelectedPatchIndex(const FName PartSlot) const
{
    if (!Session)
    {
        return INDEX_NONE;
    }

    for (const FBBBAppearancePart &Part : Session->GetDraft().Parts)
    {
        if (Part.Slot == PartSlot)
        {
            const int32 Column = FMath::Clamp(FMath::RoundToInt(Part.Patch.X * 8.0f), 0, 7);
            const int32 Row = FMath::Clamp(FMath::RoundToInt(Part.Patch.Y * 8.0f), 0, 7);
            return Row * 8 + Column;
        }
    }

    return INDEX_NONE;
}

const FSlateBrush *UBBBCharacterCustomizationView::GetItemBrush(const FName ItemId)
{
    if (ItemId.IsNone())
    {
        return nullptr;
    }

    if (const TSharedPtr<FSlateBrush> *ExistingBrush = ItemThumbnailBrushes.Find(ItemId))
    {
        return ExistingBrush->Get();
    }

    FBBBAppearanceItem Item;
    if (!Session || !Session->GetItem(ItemId, Item))
    {
        return nullptr;
    }
    if (Item.Thumbnail.IsNull())
    {
        if (Item.Mesh.IsNull() && Item.Attachments.IsEmpty())
        {
            return nullptr;
        }

        UE_LOG(LogBBBCustomizationView, Error, TEXT("外观款式缺少缩略图配置 Item=%s"), *ItemId.ToString());
        return nullptr;
    }

    UTexture2D *Texture = Item.Thumbnail.LoadSynchronous();
    if (!Texture)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("外观缩略图加载失败 Item=%s Path=%s"),
            *ItemId.ToString(), *Item.Thumbnail.ToString());
        return nullptr;
    }

    TSharedPtr<FSlateBrush> Brush = MakeShared<FSlateBrush>();
    Brush->SetResourceObject(Texture);
    Brush->ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
    ItemThumbnails.Add(ItemId, Texture);
    ItemThumbnailBrushes.Add(ItemId, Brush);
    return Brush.Get();
}

void UBBBCharacterCustomizationView::RefreshSlotCardThumbnails()
{
    if (!Session)
    {
        return;
    }

    for (TPair<FName, TSharedPtr<SBox>> &Entry : SlotThumbnailBoxes)
    {
        if (!Entry.Value.IsValid())
        {
            continue;
        }

        const FName ItemId = GetSelectedItemId(Entry.Key);
        const FSlateBrush *Thumbnail = GetItemBrush(ItemId);
        if (Thumbnail)
        {
            Entry.Value->SetContent(SNew(SImage).Image(Thumbnail));
            continue;
        }

        FBBBAppearanceItem Item;
        const bool bHasItem = !ItemId.IsNone() && Session->GetItem(ItemId, Item);
        const bool bEmptyItem = bHasItem && Item.Mesh.IsNull() && Item.Attachments.IsEmpty();
        const bool bUnconfiguredAttachment = Entry.Key == TEXT("Attachments") && bEmptyItem
            && ItemId != TEXT("Attachments_None");
        const FText Placeholder = bUnconfiguredAttachment
            ? FText::FromString(TEXT("配置未完成"))
            : ItemId.IsNone() || bEmptyItem
                ? FText::FromString(TEXT("无部件"))
                : bHasItem
                    ? FText::FromString(TEXT("缩略图缺失"))
                    : FText::FromString(TEXT("配置不可用"));
        Entry.Value->SetContent(
            SNew(STextBlock)
            .Text(Placeholder)
            .Font(GetCustomizationFont(18))
            .Justification(ETextJustify::Center)
            .ColorAndOpacity(FLinearColor(0.84f, 0.68f, 0.52f, 1.0f)));
    }
}

bool UBBBCharacterCustomizationView::IsLeftSideSlot(const FName PartSlot) const
{
    return PartSlot == TEXT("Head") || PartSlot == TEXT("Helmet")
        || PartSlot == TEXT("Body") || PartSlot == TEXT("Arms") || PartSlot == TEXT("Vest");
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSlotCard(const FName PartSlot)
{
    const FName ItemId = GetSelectedItemId(PartSlot);
    FBBBAppearanceItem CurrentItem;
    const bool bEmptyItem = !ItemId.IsNone() && Session && Session->GetItem(ItemId, CurrentItem)
        && CurrentItem.Mesh.IsNull() && CurrentItem.Attachments.IsEmpty();
    const bool bUnconfiguredAttachment = PartSlot == TEXT("Attachments") && bEmptyItem
        && ItemId != TEXT("Attachments_None");
    const FSlateBrush *Thumbnail = GetItemBrush(ItemId);
    TSharedRef<SBox> ThumbnailBox = SNew(SBox)
        .HeightOverride(98.0f)
        [
            Thumbnail
                ? StaticCastSharedRef<SWidget>(SNew(SImage).Image(Thumbnail))
                : StaticCastSharedRef<SWidget>(SNew(STextBlock)
                    .Text(bUnconfiguredAttachment
                        ? FText::FromString(TEXT("配置未完成"))
                        : ItemId.IsNone() || bEmptyItem
                            ? FText::FromString(TEXT("无部件"))
                            : FText::FromString(TEXT("缩略图缺失")))
                    .Font(GetCustomizationFont(18))
                    .Justification(ETextJustify::Center)
                    .ColorAndOpacity(FLinearColor(0.84f, 0.68f, 0.52f, 1.0f)))
        ];
    SlotThumbnailBoxes.Add(PartSlot, ThumbnailBox);

    return SNew(SButton)
        .ButtonColorAndOpacity(FLinearColor::Transparent)
        .ContentPadding(FMargin(0.0f))
        .OnHovered_Lambda([this, PartSlot]()
        {
            HoveredPartSlot = PartSlot;
            HoveredItemId = NAME_None;
            InvalidateLayoutAndVolatility();
        })
        .OnUnhovered_Lambda([this, PartSlot]()
        {
            if (HoveredPartSlot == PartSlot && HoveredItemId.IsNone())
            {
                HoveredPartSlot = NAME_None;
                InvalidateLayoutAndVolatility();
            }
        })
        .OnClicked_Lambda([this, PartSlot]()
        {
            OpenSlot(PartSlot);
            return FReply::Handled();
        })
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor_Lambda([this, PartSlot]()
            {
                if (HoveredPartSlot == PartSlot)
                {
                    return FLinearColor(0.94f, 0.55f, 0.22f, 1.0f);
                }
                if (!GetSelectedItemId(PartSlot).IsNone())
                {
                    return FLinearColor(0.62f, 0.38f, 0.17f, 0.95f);
                }
                return FLinearColor(0.30f, 0.35f, 0.42f, 0.82f);
            })
            .Padding(FMargin(1.5f))
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FLinearColor(0.014f, 0.018f, 0.026f, 0.96f))
                .Padding(FMargin(6.0f))
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    .Padding(2.0f)
                    [
                        ThumbnailBox
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(3.0f, 2.0f, 3.0f, 0.0f)
                    [
                        SNew(STextBlock)
                        .Text(GetPartSlotTitle(PartSlot))
                        .Font(GetCustomizationFont(20))
                        .ColorAndOpacity(FLinearColor(0.88f, 0.71f, 0.49f, 1.0f))
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(3.0f, 1.0f, 3.0f, 2.0f)
                    [
                        SNew(STextBlock)
                        .Text_Lambda([this, PartSlot]() { return GetSelectedItem(PartSlot); })
                        .Font(GetCustomizationFont(18))
                        .ColorAndOpacity(FLinearColor(0.94f, 0.94f, 0.93f, 1.0f))
                        .AutoWrapText(true)
                        .WrapTextAt(220.0f)
                    ]
                ]
            ]
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSlotGrid(const bool bLeft)
{
    const FBBBAppearanceSelection Draft = Session->GetDraft();
    const TArray<FName> Order = GetPartSlotOrder(bLeft);
    TArray<FName> Slots;

    for (const FName SlotName : Order)
    {
        const bool bIsConfiguredPart = Draft.Parts.ContainsByPredicate(
            [SlotName](const FBBBAppearancePart &Part) { return Part.Slot == SlotName; });
        const bool bHasCatalogItems = !Session->GetItems(SlotName).IsEmpty();
        if (bIsConfiguredPart || bHasCatalogItems)
        {
            Slots.Add(SlotName);
        }
    }

    for (const FBBBAppearancePart &Part : Draft.Parts)
    {
        if (IsLeftSideSlot(Part.Slot) == bLeft && !Slots.Contains(Part.Slot))
        {
            Slots.Add(Part.Slot);
        }
    }

    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(5.0f));
    for (int32 Index = 0; Index < Slots.Num(); ++Index)
    {
        Grid->AddSlot(Index % 2, Index / 2)
        [
            MakeSlotCard(Slots[Index])
        ];
    }

    return Grid;
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSlotStrip(const bool bLeft)
{
    TSharedRef<SHorizontalBox> Strip = SNew(SHorizontalBox);
    for (const FName SlotName : GetPartSlotOrder(bLeft))
    {
        const bool bHasCatalogItems = !Session->GetItems(SlotName).IsEmpty();
        const bool bHasDraftPart = Session->GetDraft().Parts.ContainsByPredicate(
            [SlotName](const FBBBAppearancePart &Part) { return Part.Slot == SlotName; });
        if (!bHasCatalogItems && !bHasDraftPart)
        {
            continue;
        }

        Strip->AddSlot()
        .FillWidth(1.0f)
        .Padding(FMargin(2.0f, 0.0f))
        [
            SNew(SButton)
            .ButtonColorAndOpacity_Lambda([this, SlotName]()
            {
                return FSlateColor(ExpandedSlot == SlotName
                    ? FLinearColor(0.54f, 0.31f, 0.15f, 1.0f) : FLinearColor(0.09f, 0.11f, 0.14f, 0.94f));
            })
            .ContentPadding(FMargin(4.0f, 4.0f))
            .TextStyle(&GetCustomizationButtonTextStyle())
            .Text(GetPartSlotTitle(SlotName))
            .OnClicked_Lambda([this, SlotName]()
            {
                OpenSlot(SlotName);
                return FReply::Handled();
            })
        ];
    }

    return Strip;
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeItemCard(const FName PartSlot, const FName ItemId)
{
    FBBBAppearanceItem Item;
    if (!Session->GetItem(ItemId, Item) || Item.Slot != PartSlot)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("外观网格中出现无效条目 Slot=%s Item=%s"),
            *PartSlot.ToString(), *ItemId.ToString());
        return SNew(STextBlock).Text(FText::FromString(TEXT("款式配置错误")));
    }

    const FSlateBrush *Thumbnail = GetItemBrush(ItemId);
    const bool bEmptyItem = Item.Mesh.IsNull() && Item.Attachments.IsEmpty();
    const bool bUnconfiguredAttachment = PartSlot == TEXT("Attachments") && bEmptyItem
        && ItemId != TEXT("Attachments_None");
    if (bUnconfiguredAttachment)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("附件组合没有挂载配置 Item=%s"), *ItemId.ToString());
    }

    return SNew(SButton)
        .IsEnabled(!bUnconfiguredAttachment)
        .ButtonColorAndOpacity(FLinearColor::Transparent)
        .ContentPadding(FMargin(0.0f))
        .OnHovered_Lambda([this, PartSlot, ItemId]()
        {
            HoveredPartSlot = PartSlot;
            HoveredItemId = ItemId;
            InvalidateLayoutAndVolatility();
        })
        .OnUnhovered_Lambda([this, PartSlot, ItemId]()
        {
            if (HoveredPartSlot == PartSlot && HoveredItemId == ItemId)
            {
                HoveredPartSlot = NAME_None;
                HoveredItemId = NAME_None;
                InvalidateLayoutAndVolatility();
            }
        })
        .OnClicked_Lambda([this, PartSlot, ItemId]()
        {
            if (Session && Session->SelectItem(PartSlot, ItemId))
            {
                RefreshSlotCardThumbnails();
                StatusMessage = FText::FromString(TEXT("已预览  应用后保存"));
                InvalidateLayoutAndVolatility();
            }
            if (!Session)
            {
                UE_LOG(LogBBBCustomizationView, Error, TEXT("换装会话已失效 Slot=%s Item=%s"),
                    *PartSlot.ToString(), *ItemId.ToString());
            }
            return FReply::Handled();
        })
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor_Lambda([this, PartSlot, ItemId]()
            {
                if (HoveredPartSlot == PartSlot && HoveredItemId == ItemId)
                {
                    return FLinearColor(0.98f, 0.60f, 0.25f, 1.0f);
                }
                if (GetSelectedItemId(PartSlot) == ItemId)
                {
                    return FLinearColor(0.68f, 0.39f, 0.15f, 1.0f);
                }
                return FLinearColor(0.34f, 0.39f, 0.46f, 0.78f);
            })
            .Padding(FMargin(1.5f))
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FLinearColor(0.013f, 0.017f, 0.024f, 0.97f))
                .Padding(FMargin(5.0f))
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    .Padding(2.0f)
                    [
                        SNew(SBox)
                        .HeightOverride(118.0f)
                        [
                            Thumbnail
                            ? StaticCastSharedRef<SWidget>(SNew(SImage).Image(Thumbnail))
                                : StaticCastSharedRef<SWidget>(SNew(STextBlock)
                                    .Text(bUnconfiguredAttachment
                                        ? FText::FromString(TEXT("配置未完成"))
                                        : bEmptyItem
                                            ? FText::FromString(TEXT("无部件"))
                                            : FText::FromString(TEXT("缩略图缺失")))
                                    .Font(GetCustomizationFont(18))
                                    .Justification(ETextJustify::Center)
                                    .ColorAndOpacity(FLinearColor(0.84f, 0.68f, 0.52f, 1.0f)))
                        ]
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(2.0f, 4.0f, 2.0f, 2.0f)
                    [
                        SNew(STextBlock)
                        .Text(Item.DisplayName)
                        .Font(GetCustomizationFont(18))
                        .ColorAndOpacity(FLinearColor(0.94f, 0.94f, 0.93f, 1.0f))
                        .AutoWrapText(true)
                        .WrapTextAt(150.0f)
                    ]
                ]
            ]
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeItemGrid(const FName PartSlot)
{
    const TArray<FName> Items = Session->GetItems(PartSlot);
    TSharedRef<SVerticalBox> Panel = SNew(SVerticalBox);

    Panel->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 7.0f)
    [
        MakeSlotStrip(IsLeftSideSlot(PartSlot))
    ];

    Panel->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 8.0f)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(1.0f)
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text_Lambda([this, PartSlot]() { return GetPartSlotTitle(PartSlot); })
            .Font(GetCustomizationFont(24))
            .ColorAndOpacity(FLinearColor(0.91f, 0.77f, 0.56f, 1.0f))
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text_Lambda([this, PartSlot]()
            {
                const TArray<FName> Available = Session ? Session->GetItems(PartSlot) : TArray<FName>();
                return FText::AsNumber(Available.Num());
            })
            .Font(GetCustomizationFont(18))
            .ColorAndOpacity(FLinearColor(0.58f, 0.62f, 0.67f, 1.0f))
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(7.0f, 0.0f, 0.0f, 0.0f)
        [
            SNew(SButton)
            .ButtonColorAndOpacity(FLinearColor(0.09f, 0.11f, 0.14f, 0.94f))
            .ContentPadding(FMargin(7.0f, 4.0f))
            .TextStyle(&GetCustomizationButtonTextStyle())
            .Text(FText::FromString(TEXT("返回部位")))
            .OnClicked_Lambda([this]()
            {
                CloseSlot();
                return FReply::Handled();
            })
        ]
    ];

    if (PartSlot == TEXT("Body") || PartSlot == TEXT("Vest"))
    {
        Panel->AddSlot()
        .AutoHeight()
        .Padding(0.0f, 0.0f, 0.0f, 8.0f)
        [
            SNew(SButton)
            .ButtonColorAndOpacity(FLinearColor(0.11f, 0.13f, 0.16f, 0.96f))
            .ContentPadding(FMargin(10.0f, 6.0f))
            .TextStyle(&GetCustomizationButtonTextStyle())
            .Text(FText::FromString(TEXT("选择徽章图案")))
            .OnClicked_Lambda([this, PartSlot]()
            {
                ShowPatchGrid(PartSlot);
                return FReply::Handled();
            })
        ];
    }

    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(5.0f));
    for (int32 Index = 0; Index < Items.Num(); ++Index)
    {
        Grid->AddSlot(Index % 3, Index / 3)
        [
            MakeItemCard(PartSlot, Items[Index])
        ];
    }

    if (Items.IsEmpty())
    {
        Panel->AddSlot()
        .AutoHeight()
        .Padding(0.0f, 10.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("当前部位没有可选款式")))
            .Font(GetCustomizationFont(18))
            .ColorAndOpacity(FLinearColor(0.72f, 0.74f, 0.77f, 1.0f))
        ];
    }

    Panel->AddSlot()
    .FillHeight(1.0f)
    [
        SNew(SScrollBox)
        + SScrollBox::Slot()
        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
        [
            Grid
        ]
    ];

    return Panel;
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSidePanel(const bool bLeft)
{
    TSharedRef<SBox> SlotPanel = SNew(SBox)
        .Visibility_Lambda([this, bLeft]()
        {
            return ExpandedSlot.IsNone() || IsLeftSideSlot(ExpandedSlot) != bLeft
                ? EVisibility::Visible : EVisibility::Collapsed;
        })
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .Padding(0.0f, 0.0f, 4.0f, 0.0f)
            [
                MakeSlotGrid(bLeft)
            ]
        ];

    TSharedRef<SBox> ItemPanel = SNew(SBox)
        .Visibility_Lambda([this, bLeft]()
        {
            return !ExpandedSlot.IsNone() && IsLeftSideSlot(ExpandedSlot) == bLeft && !bShowingPatches
                ? EVisibility::Visible : EVisibility::Collapsed;
        });

    TSharedRef<SBox> PatchPanel = SNew(SBox)
        .Visibility_Lambda([this, bLeft]()
        {
            return !ExpandedSlot.IsNone() && IsLeftSideSlot(ExpandedSlot) == bLeft && bShowingPatches
                ? EVisibility::Visible : EVisibility::Collapsed;
        });

    if (bLeft)
    {
        LeftItemGrid = ItemPanel;
        LeftPatchGrid = PatchPanel;
    }
    if (!bLeft)
    {
        RightItemGrid = ItemPanel;
        RightPatchGrid = PatchPanel;
    }

    TSharedRef<SVerticalBox> Contents = SNew(SVerticalBox);
    Contents->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 9.0f)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(FText::FromString(bLeft ? TEXT("上身与护具") : TEXT("下身与携行")))
            .Font(GetCustomizationFont(27))
            .ColorAndOpacity(FLinearColor(0.92f, 0.83f, 0.70f, 1.0f))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(0.0f, 2.0f, 0.0f, 0.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("当前穿戴部位")))
            .Font(GetCustomizationFont(18))
            .ColorAndOpacity(FLinearColor(0.68f, 0.70f, 0.72f, 1.0f))
        ]
    ];

    Contents->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 8.0f)
    [
        SNew(SBox)
        .HeightOverride(1.0f)
        [
            SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(FLinearColor(0.35f, 0.39f, 0.46f, 0.72f))
        ]
    ];

    Contents->AddSlot()
    .FillHeight(1.0f)
    [
        SNew(SOverlay)
        + SOverlay::Slot()
        [
            SlotPanel
        ]
        + SOverlay::Slot()
        [
            ItemPanel
        ]
        + SOverlay::Slot()
        [
            PatchPanel
        ]
    ];

    if (bLeft)
    {
        Contents->AddSlot()
        .AutoHeight()
        .Padding(0.0f, 6.0f, 0.0f, 0.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SButton)
                .ButtonColorAndOpacity(FLinearColor(0.09f, 0.10f, 0.12f, 0.9f))
                .ContentPadding(FMargin(8.0f, 5.0f))
                .TextStyle(&GetCustomizationButtonTextStyle())
                .Text(FText::FromString(TEXT("表面状态")))
                .OnClicked_Lambda([this]()
                {
                    bSurfaceExpanded = !bSurfaceExpanded;
                    InvalidateLayoutAndVolatility();
                    return FReply::Handled();
                })
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 5.0f, 0.0f, 0.0f)
            [
                SNew(SBox)
                .Visibility_Lambda([this]()
                {
                    return bSurfaceExpanded && ExpandedSlot.IsNone()
                        ? EVisibility::Visible : EVisibility::Collapsed;
                })
                [
                    MakeSurfaceControls()
                ]
            ]
        ];
    }

    return SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.012f, 0.015f, 0.022f, 0.62f))
        .Padding(FMargin(14.0f, 10.0f))
        [
            Contents
        ];
}

void UBBBCharacterCustomizationView::UpdatePatchPreview(const FName PartSlot)
{
    UMaterialInstanceDynamic *Material = PartSlot == TEXT("Body") ? BodyPatchMaterial.Get() : VestPatchMaterial.Get();
    if (!Session || !Material)
    {
        return;
    }

    for (const FBBBAppearancePart &Part : Session->GetDraft().Parts)
    {
        if (Part.Slot == PartSlot)
        {
            Material->SetVectorParameterValue(TEXT("Coord"), FLinearColor(Part.Patch.X, Part.Patch.Y, 0.0f, 0.0f));
            return;
        }
    }
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakePatchRow(const FName PartSlot)
{
    TObjectPtr<UMaterialInstanceDynamic> &Material = PartSlot == TEXT("Body") ? BodyPatchMaterial : VestPatchMaterial;
    FSlateBrush &Brush = PartSlot == TEXT("Body") ? BodyPatchBrush : VestPatchBrush;
    UMaterialInterface *BaseMaterial = PatchPreviewMaterial.LoadSynchronous();
    if (BaseMaterial)
    {
        Material = UMaterialInstanceDynamic::Create(BaseMaterial, this);
    }
    if (!Material)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("徽章预览材质不可用 Path=%s"), *PatchPreviewMaterial.ToString());
    }

    Brush.SetResourceObject(Material);
    Brush.ImageSize = FVector2D(72.0f, 54.0f);
    UpdatePatchPreview(PartSlot);

    if (!PatchAtlas)
    {
        PatchAtlas = LoadObject<UTexture2D>(nullptr,
            TEXT("/Game/UkraineSoldier/Textures/Flags/T_Flags_BC.T_Flags_BC"));
    }
    if (!PatchAtlas)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("徽章图案图集不可用"));
    }

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(0.0f, 0.0f, 0.0f, 8.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 0.0f, 10.0f, 0.0f)
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FLinearColor(0.08f, 0.09f, 0.11f, 0.9f))
                .Padding(FMargin(3.0f))
                [
                    SNew(SImage).Image(&Brush)
                ]
            ]
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString(PartSlot == TEXT("Body") ? TEXT("身体徽章") : TEXT("背心徽章")))
                .Font(GetCustomizationFont(20))
                .ColorAndOpacity(FLinearColor(0.91f, 0.85f, 0.77f, 1.0f))
            ]
        ]
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            MakePatchGrid(PartSlot)
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakePatchGrid(const FName PartSlot)
{
    PatchBrushes.SetNum(64);
    if (PatchAtlas)
    {
        for (int32 Index = 0; Index < PatchBrushes.Num(); ++Index)
        {
            const int32 Column = Index % 8;
            const int32 Row = Index / 8;
            FSlateBrush &Brush = PatchBrushes[Index];
            Brush.SetResourceObject(PatchAtlas);
            Brush.ImageSize = FVector2D(48.0f, 48.0f);
            Brush.SetUVRegion(FBox2d(
                FVector2d(static_cast<double>(Column) / 8.0, static_cast<double>(Row) / 8.0),
                FVector2d(static_cast<double>(Column + 1) / 8.0, static_cast<double>(Row + 1) / 8.0)));
        }
    }

    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(3.0f));
    for (int32 Index = 0; Index < 64; ++Index)
    {
        Grid->AddSlot(Index % 8, Index / 8)
        [
            SNew(SButton)
            .ButtonColorAndOpacity(FLinearColor::White)
            .ContentPadding(FMargin(0.0f))
            .OnClicked_Lambda([this, PartSlot, Index]()
            {
                if (Session && Session->SelectPatch(PartSlot, Index))
                {
                    UpdatePatchPreview(PartSlot);
                    StatusMessage = FText::FromString(TEXT("徽章图案已预览  应用后保存"));
                }
                return FReply::Handled();
            })
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor_Lambda([this, PartSlot, Index]()
                {
                    const FLinearColor Selected(0.90f, 0.49f, 0.20f, 1.0f);
                    const FLinearColor Normal(0.20f, 0.22f, 0.25f, 0.9f);
                    return GetSelectedPatchIndex(PartSlot) == Index ? Selected : Normal;
                })
                .Padding(FMargin(2.0f))
                [
                    SNew(SBox)
                    .WidthOverride(46.0f)
                    .HeightOverride(46.0f)
                    [
                        PatchAtlas
                            ? StaticCastSharedRef<SWidget>(SNew(SImage).Image(&PatchBrushes[Index]))
                            : StaticCastSharedRef<SWidget>(SNew(STextBlock)
                                .Text(FText::AsNumber(Index + 1))
                                .Font(GetCustomizationFont(16))
                                .Justification(ETextJustify::Center))
                    ]
                ]
            ]
        ];
    }

    return SNew(SScrollBox)
        + SScrollBox::Slot()
        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
        [
            Grid
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSurfaceControls()
{
    return SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.035f, 0.041f, 0.050f, 0.88f))
        .Padding(FMargin(9.0f, 6.0f))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("污渍")))
                    .Font(GetCustomizationFont(18))
                    .ColorAndOpacity(FLinearColor(0.89f, 0.90f, 0.91f, 1.0f))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::AsPercent(Session ? Session->GetDraft().Dirt : 0.0f);
                    })
                    .Font(GetCustomizationFont(18))
                    .ColorAndOpacity(FLinearColor(0.67f, 0.69f, 0.71f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 0.0f, 0.0f, 5.0f)
            [
                SNew(SSlider)
                .Value_Lambda([this]()
                {
                    return Session ? Session->GetDraft().Dirt : 0.0f;
                })
                .OnValueChanged_Lambda([this](const float Value)
                {
                    if (Session)
                    {
                        Session->SetSurface(Value, Session->GetDraft().Weathering);
                    }
                })
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("磨损")))
                    .Font(GetCustomizationFont(18))
                    .ColorAndOpacity(FLinearColor(0.89f, 0.90f, 0.91f, 1.0f))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::AsPercent(Session ? Session->GetDraft().Weathering : 0.0f);
                    })
                    .Font(GetCustomizationFont(18))
                    .ColorAndOpacity(FLinearColor(0.67f, 0.69f, 0.71f, 1.0f))
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SSlider)
                .Value_Lambda([this]()
                {
                    return Session ? Session->GetDraft().Weathering : 0.0f;
                })
                .OnValueChanged_Lambda([this](const float Value)
                {
                    if (Session)
                    {
                        Session->SetSurface(Session->GetDraft().Dirt, Value);
                    }
                })
            ]
        ];
}

void UBBBCharacterCustomizationView::OpenSlot(const FName PartSlot)
{
    if (!Session || Session->GetItems(PartSlot).IsEmpty())
    {
        UE_LOG(LogBBBCustomizationView, Warning, TEXT("无法打开没有候选款式的部位 Slot=%s"), *PartSlot.ToString());
        return;
    }

    ExpandedSlot = PartSlot;
    bShowingPatches = false;
    if (LeftPatchGrid)
    {
        LeftPatchGrid->SetContent(SNullWidget::NullWidget);
    }
    if (RightPatchGrid)
    {
        RightPatchGrid->SetContent(SNullWidget::NullWidget);
    }
    if (IsLeftSideSlot(PartSlot) && LeftItemGrid)
    {
        LeftItemGrid->SetContent(MakeItemGrid(PartSlot));
    }
    if (!IsLeftSideSlot(PartSlot) && RightItemGrid)
    {
        RightItemGrid->SetContent(MakeItemGrid(PartSlot));
    }
    InvalidateLayoutAndVolatility();
}

void UBBBCharacterCustomizationView::ShowPatchGrid(const FName PartSlot)
{
    if (PartSlot != TEXT("Body") && PartSlot != TEXT("Vest"))
    {
        UE_LOG(LogBBBCustomizationView, Warning, TEXT("徽章入口不支持该部位 Slot=%s"), *PartSlot.ToString());
        return;
    }

    ExpandedSlot = PartSlot;
    bShowingPatches = true;
    TSharedRef<SVerticalBox> Panel = SNew(SVerticalBox);
    Panel->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 7.0f)
    [
        MakeSlotStrip(IsLeftSideSlot(PartSlot))
    ];
    Panel->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 0.0f, 0.0f, 7.0f)
    [
        SNew(SButton)
        .ButtonColorAndOpacity(FLinearColor(0.09f, 0.11f, 0.14f, 0.94f))
        .ContentPadding(FMargin(7.0f, 4.0f))
        .TextStyle(&GetCustomizationButtonTextStyle())
        .Text(FText::FromString(TEXT("返回款式")))
        .OnClicked_Lambda([this, PartSlot]()
        {
            OpenSlot(PartSlot);
            return FReply::Handled();
        })
    ];
    Panel->AddSlot()
    .FillHeight(1.0f)
    [
        MakePatchRow(PartSlot)
    ];

    if (IsLeftSideSlot(PartSlot) && LeftPatchGrid)
    {
        LeftPatchGrid->SetContent(Panel);
    }
    if (!IsLeftSideSlot(PartSlot) && RightPatchGrid)
    {
        RightPatchGrid->SetContent(Panel);
    }
    InvalidateLayoutAndVolatility();
}

void UBBBCharacterCustomizationView::CloseSlot()
{
    ExpandedSlot = NAME_None;
    bShowingPatches = false;
    if (LeftItemGrid)
    {
        LeftItemGrid->SetContent(SNullWidget::NullWidget);
    }
    if (RightItemGrid)
    {
        RightItemGrid->SetContent(SNullWidget::NullWidget);
    }
    if (LeftPatchGrid)
    {
        LeftPatchGrid->SetContent(SNullWidget::NullWidget);
    }
    if (RightPatchGrid)
    {
        RightPatchGrid->SetContent(SNullWidget::NullWidget);
    }
    InvalidateLayoutAndVolatility();
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::RebuildWidget()
{
    if (!Session)
    {
        return SNew(STextBlock).Text(FText::FromString(TEXT("换装会话不可用")));
    }

    SlotThumbnailBoxes.Reset();
    PatchAtlas = LoadObject<UTexture2D>(nullptr,
        TEXT("/Game/UkraineSoldier/Textures/Flags/T_Flags_BC.T_Flags_BC"));
    if (!PatchAtlas)
    {
        UE_LOG(LogBBBCustomizationView, Error, TEXT("身体与背心徽章图集加载失败"));
    }

    TSharedRef<SConstraintCanvas> Layout = SNew(SConstraintCanvas);
    Layout->AddSlot()
    .Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
    [
        SNew(SImage).Image(&PreviewBrush)
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
    [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.005f, 0.008f, 0.014f, 0.13f))
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.025f, 0.105f, 0.315f, 0.86f))
    .Offset(FMargin(0.0f))
    [
        MakeSidePanel(true)
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.685f, 0.105f, 0.975f, 0.86f))
    .Offset(FMargin(0.0f))
    [
        MakeSidePanel(false)
    ];

    Layout->AddSlot()
    .Anchors(FAnchors(0.18f, 0.895f, 0.82f, 0.985f))
    .Offset(FMargin(0.0f))
    [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.018f, 0.021f, 0.027f, 0.78f))
        .Padding(FMargin(6.0f, 4.0f))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2.0f)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.10f, 0.12f, 0.15f, 0.95f))
                    .ContentPadding(FMargin(6.0f, 4.0f))
                    .TextStyle(&GetCustomizationButtonTextStyle())
                    .Text(FText::FromString(TEXT("转左")))
                    .OnClicked_Lambda([this]()
                    {
                        if (Session)
                        {
                            Session->RotatePreview(-15.0f);
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(2.0f)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity_Lambda([this]()
                    {
                        return FSlateColor(CurrentView == TEXT("Full")
                            ? FLinearColor(0.78f, 0.40f, 0.17f, 1.0f) : FLinearColor(0.10f, 0.12f, 0.15f, 0.95f));
                    })
                    .ContentPadding(FMargin(6.0f, 4.0f))
                    .TextStyle(&GetCustomizationButtonTextStyle())
                    .Text(FText::FromString(TEXT("全身")))
                    .OnClicked_Lambda([this]()
                    {
                        CurrentView = TEXT("Full");
                        if (Session)
                        {
                            Session->SelectView(CurrentView);
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(2.0f)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity_Lambda([this]()
                    {
                        return FSlateColor(CurrentView == TEXT("Head")
                            ? FLinearColor(0.78f, 0.40f, 0.17f, 1.0f) : FLinearColor(0.10f, 0.12f, 0.15f, 0.95f));
                    })
                    .ContentPadding(FMargin(6.0f, 4.0f))
                    .TextStyle(&GetCustomizationButtonTextStyle())
                    .Text(FText::FromString(TEXT("头部")))
                    .OnClicked_Lambda([this]()
                    {
                        CurrentView = TEXT("Head");
                        if (Session)
                        {
                            Session->SelectView(CurrentView);
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(2.0f)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity_Lambda([this]()
                    {
                        return FSlateColor(CurrentView == TEXT("Legs")
                            ? FLinearColor(0.78f, 0.40f, 0.17f, 1.0f) : FLinearColor(0.10f, 0.12f, 0.15f, 0.95f));
                    })
                    .ContentPadding(FMargin(6.0f, 4.0f))
                    .TextStyle(&GetCustomizationButtonTextStyle())
                    .Text(FText::FromString(TEXT("腿部")))
                    .OnClicked_Lambda([this]()
                    {
                        CurrentView = TEXT("Legs");
                        if (Session)
                        {
                            Session->SelectView(CurrentView);
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2.0f)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.10f, 0.12f, 0.15f, 0.95f))
                    .ContentPadding(FMargin(6.0f, 4.0f))
                    .TextStyle(&GetCustomizationButtonTextStyle())
                    .Text(FText::FromString(TEXT("转右")))
                    .OnClicked_Lambda([this]()
                    {
                        if (Session)
                        {
                            Session->RotatePreview(15.0f);
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(2.0f)
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return StatusMessage;
                    })
                    .Font(GetCustomizationFont(18))
                    .ColorAndOpacity(FLinearColor(0.81f, 0.78f, 0.72f, 1.0f))
                    .Justification(ETextJustify::Center)
                    .AutoWrapText(true)
                    .WrapTextAt(190.0f)
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2.0f)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.80f, 0.37f, 0.13f, 1.0f))
                    .ContentPadding(FMargin(8.0f, 4.0f))
                    .TextStyle(&GetCustomizationButtonTextStyle())
                    .Text(FText::FromString(TEXT("应用")))
                    .OnClicked_Lambda([this]()
                    {
                        if (Session)
                        {
                            const bool bApplied = Session->Apply();
                            StatusMessage = FText::FromString(bApplied
                                ? TEXT("外观已应用并保存") : TEXT("处理失败  请查看日志"));
                            InvalidateLayoutAndVolatility();
                        }
                        return FReply::Handled();
                    })
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(2.0f)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.10f, 0.12f, 0.15f, 0.95f))
                    .ContentPadding(FMargin(8.0f, 4.0f))
                    .TextStyle(&GetCustomizationButtonTextStyle())
                    .Text(FText::FromString(TEXT("取消")))
                    .OnClicked_Lambda([this]()
                    {
                        if (Session)
                        {
                            Session->Close();
                        }
                        return FReply::Handled();
                    })
                ]
            ]
        ]
    ];

    return Layout;
}

FReply UBBBCharacterCustomizationView::NativeOnKeyDown(
    const FGeometry &Geometry,
    const FKeyEvent &KeyEvent)
{
    if (!Session)
    {
        return Super::NativeOnKeyDown(Geometry, KeyEvent);
    }

    if (KeyEvent.GetKey() == EKeys::F6)
    {
        Session->Close();
        return FReply::Handled();
    }

    if (KeyEvent.GetKey() == EKeys::Escape)
    {
        if (bShowingPatches)
        {
            OpenSlot(ExpandedSlot);
            return FReply::Handled();
        }
        if (!ExpandedSlot.IsNone())
        {
            CloseSlot();
            return FReply::Handled();
        }

        Session->Close();
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(Geometry, KeyEvent);
}
