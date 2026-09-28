#include "BBBCharacterCustomizationView.h"
#include "BBBCharacterCustomizationStyle.h"
#include "BBBWork/UBBBNexus/Customization/BBBCharacterCustomizationSession.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

using namespace BBBCustomizationStyle;

DEFINE_LOG_CATEGORY_STATIC(LogBBBCustomizationCards, Log, All);

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSlotCard(const FName PartSlot)
{
    const FName ItemId = GetSelectedItemId(PartSlot);
    FBBBAppearanceItem CurrentItem;
    const bool bEmptyItem = !ItemId.IsNone() && Session && Session->GetItem(ItemId, CurrentItem)
        && CurrentItem.Mesh.IsNull() && CurrentItem.Attachments.IsEmpty();
    const bool bUnconfiguredAttachment = PartSlot == TEXT("Attachments") && bEmptyItem
        && ItemId != TEXT("Attachments_None");
    const FSlateBrush *Thumbnail = GetItemBrush(ItemId);
    FText Placeholder = FText::FromString(TEXT("缩略图缺失"));
    if (ItemId.IsNone() || bEmptyItem)
    {
        Placeholder = FText::FromString(TEXT("无部件"));
    }
    if (bUnconfiguredAttachment)
    {
        Placeholder = FText::FromString(TEXT("配置未完成"));
    }

    TSharedRef<SBox> ThumbnailBox = SNew(SBox)
        .HeightOverride(108.0f)
        [
            Thumbnail
                ? StaticCastSharedRef<SWidget>(SNew(SScaleBox).Stretch(EStretch::ScaleToFit)[SNew(SImage).Image(Thumbnail)])
                : StaticCastSharedRef<SWidget>(SNew(STextBlock)
                    .Text(Placeholder)
                    .Font(GetCustomizationFont(18))
                    .Justification(ETextJustify::Center)
                    .ColorAndOpacity(FLinearColor(0.84f, 0.68f, 0.52f, 1.0f)))
        ];
    SlotThumbnailBoxes.Add(PartSlot, ThumbnailBox);

    return SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ToolTipText_Lambda([this, PartSlot]() { return GetSelectedItem(PartSlot); })
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
            .BorderImage(&CardFrameBrush)
            .BorderBackgroundColor_Lambda([this, PartSlot]()
            {
                if (HoveredPartSlot == PartSlot)
                {
                    return FLinearColor(0.94f, 0.55f, 0.22f, 1.0f);
                }
                if (!GetSelectedItemId(PartSlot).IsNone())
                {
                    return FLinearColor(0.48f, 0.46f, 0.41f, 0.8f);
                }
                return FLinearColor(0.30f, 0.35f, 0.42f, 0.82f);
            })
            .Padding(FMargin(3.0f))
            [
                SNew(SBorder)
                .BorderImage(&CardSurfaceBrush)
                .BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 0.78f))
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
                        .Font(GetCustomizationFont(17))
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
                        .OverflowPolicy(ETextOverflowPolicy::Ellipsis)
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
        .ButtonStyle(&ActionButtonStyle)
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
        UE_LOG(LogBBBCustomizationCards, Error, TEXT("外观网格中出现无效条目 Slot=%s Item=%s"),
            *PartSlot.ToString(), *ItemId.ToString());
        return SNew(STextBlock).Text(FText::FromString(TEXT("款式配置错误")));
    }

    const FSlateBrush *Thumbnail = GetItemBrush(ItemId);
    const bool bEmptyItem = Item.Mesh.IsNull() && Item.Attachments.IsEmpty();
    const bool bUnconfiguredAttachment = PartSlot == TEXT("Attachments") && bEmptyItem
        && ItemId != TEXT("Attachments_None");
    if (bUnconfiguredAttachment)
    {
        UE_LOG(LogBBBCustomizationCards, Error, TEXT("附件组合没有挂载配置 Item=%s"), *ItemId.ToString());
    }

    FText Placeholder = FText::FromString(TEXT("缩略图缺失"));
    if (bEmptyItem)
    {
        Placeholder = FText::FromString(TEXT("无部件"));
    }
    if (bUnconfiguredAttachment)
    {
        Placeholder = FText::FromString(TEXT("配置未完成"));
    }

    return SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
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
            if (!Session)
            {
                UE_LOG(LogBBBCustomizationCards, Error, TEXT("换装会话已失效 Slot=%s Item=%s"),
                    *PartSlot.ToString(), *ItemId.ToString());
                return FReply::Handled();
            }
            if (!Session->SelectItem(PartSlot, ItemId))
            {
                UE_LOG(LogBBBCustomizationCards, Warning, TEXT("款式预览失败 Slot=%s Item=%s"),
                    *PartSlot.ToString(), *ItemId.ToString());
                StatusMessage = FText::FromString(TEXT("款式预览失败  保留原选择"));
                return FReply::Handled();
            }

            RefreshSlotCardThumbnails();
            StatusMessage = FText::FromString(TEXT("已预览  应用后保存"));
            InvalidateLayoutAndVolatility();
            return FReply::Handled();
        })
        [
            SNew(SBorder)
            .BorderImage(&CardFrameBrush)
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
            .Padding(FMargin(3.0f))
            [
                SNew(SBorder)
                .BorderImage(&CardSurfaceBrush)
                .BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 0.80f))
                .Padding(FMargin(5.0f))
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .FillHeight(1.0f)
                    .Padding(2.0f)
                    [
                        SNew(SBox)
                        .HeightOverride(110.0f)
                        [
                            Thumbnail
                            ? StaticCastSharedRef<SWidget>(SNew(SScaleBox).Stretch(EStretch::ScaleToFit)[SNew(SImage).Image(Thumbnail)])
                                : StaticCastSharedRef<SWidget>(SNew(STextBlock)
                                    .Text(Placeholder)
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
        .ButtonStyle(&ActionButtonStyle)
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
        .ButtonStyle(&ActionButtonStyle)
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
        .Style(&ItemScrollBoxStyle)
        .ScrollBarStyle(&ItemScrollBarStyle)
        .ScrollWhenFocusChanges(EScrollWhenFocusChanges::AnimatedScroll)
        + SScrollBox::Slot()
        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
        [
            Grid
        ]
    ];

    return Panel;
}
