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
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

using namespace BBBCustomizationStyle;

DEFINE_LOG_CATEGORY_STATIC(LogBBBCustomizationCards, Log, All);

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSlotCard(const FName PartSlot)
{
    TSharedRef<SBox> ThumbnailBox = SNew(SBox);
    SlotThumbnailBoxes.Add(PartSlot, ThumbnailBox);
    return SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ToolTipText_Lambda([this, PartSlot]() { return GetSelectedItem(PartSlot); })
        .ContentPadding(0.0f)
        .OnHovered_Lambda([this, PartSlot]()
        {
            HoveredPartSlot = PartSlot;
            HoveredItemId = NAME_None;
        })
        .OnUnhovered_Lambda([this, PartSlot]()
        {
            if (HoveredPartSlot == PartSlot)
            {
                HoveredPartSlot = NAME_None;
            }
        })
        .OnClicked_Lambda([this, PartSlot]()
        {
            OpenSlot(PartSlot);
            return FReply::Handled();
        })
        [
            SNew(SBorder)
            .BorderImage_Lambda([this, PartSlot]()
            {
                return GetCardBrush(false, HoveredPartSlot == PartSlot);
            })
            .Padding(1.0f)
            [
                SNew(SBox)
                .HeightOverride_Lambda([this, PartSlot]()
                {
                    return bSurfaceExpanded && ExpandedSlot.IsNone() && IsLeftSideSlot(PartSlot)
                        ? 167.0f : 195.0f;
                })
                [
                    SNew(SOverlay)
                    + SOverlay::Slot()
                    [
                        SNew(SImage)
                        .Image(&CardFrameBrush)
                        .ColorAndOpacity(FLinearColor(0.30f, 0.34f, 0.36f, 0.09f))
                        .Visibility(EVisibility::HitTestInvisible)
                    ]
                    + SOverlay::Slot()
                    [
                        SNew(SImage)
                        .Image(&CardSurfaceBrush)
                        .ColorAndOpacity(FLinearColor(0.16f, 0.18f, 0.20f, 0.12f))
                        .Visibility(EVisibility::HitTestInvisible)
                    ]
                    + SOverlay::Slot()
                    .Padding(8.0f, 4.0f, 8.0f, 27.0f)
                    [
                        ThumbnailBox
                    ]
                    + SOverlay::Slot()
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Top)
                    .Padding(10.0f)
                    [
                        MakeIcon(PartSlot, 22.0f, MutedInk)
                    ]
                    + SOverlay::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Top)
                    .Padding(10.0f)
                    [
                        SNew(SBBBCharacterCustomizationIcon)
                        .Symbol(TEXT("Apply"))
                        .Size(15.0f)
                        .Color(FLinearColor(0.60f, 0.64f, 0.63f))
                        .Visibility_Lambda([this, PartSlot]()
                        {
                            return GetSelectedItemId(PartSlot).IsNone()
                                ? EVisibility::Collapsed : EVisibility::HitTestInvisible;
                        })
                    ]
                    + SOverlay::Slot()
                    .VAlign(VAlign_Bottom)
                    .Padding(10.0f, 0.0f, 10.0f, 9.0f)
                    [
                        SNew(STextBlock)
                        .Text_Lambda([this, PartSlot]() { return GetSelectedItem(PartSlot); })
                        .Font(GetCustomizationFont(12))
                        .ColorAndOpacity(Ink)
                        .OverflowPolicy(ETextOverflowPolicy::Ellipsis)
                        .Visibility(EVisibility::HitTestInvisible)
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
            [SlotName](const FBBBAppearancePart& Part) { return Part.Slot == SlotName; });
        if (bIsConfiguredPart || !Session->GetItems(SlotName).IsEmpty())
        {
            Slots.Add(SlotName);
        }
    }
    for (const FBBBAppearancePart& Part : Draft.Parts)
    {
        if (IsLeftSideSlot(Part.Slot) == bLeft && !Slots.Contains(Part.Slot))
        {
            Slots.Add(Part.Slot);
        }
    }
    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(5.0f));
    for (int32 Index = 0; Index < Slots.Num(); ++Index)
    {
        Grid->AddSlot(Index % 2, Index / 2)[MakeSlotCard(Slots[Index])];
    }
    return Grid;
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeSlotStrip(const bool bLeft)
{
    TSharedRef<SHorizontalBox> Strip = SNew(SHorizontalBox);
    for (const FName SlotName : GetPartSlotOrder(bLeft))
    {
        if (Session->GetItems(SlotName).IsEmpty())
        {
            continue;
        }
        Strip->AddSlot()
        .FillWidth(1.0f)
        .Padding(3.0f, 0.0f)
        [
            SNew(SButton)
            .ButtonStyle(&ActionButtonStyle)
            .ToolTipText(GetPartSlotTitle(SlotName))
            .ContentPadding(0.0f)
            .OnClicked_Lambda([this, SlotName]()
            {
                OpenSlot(SlotName);
                return FReply::Handled();
            })
            [
                SNew(SBorder)
                .BorderImage_Lambda([this, SlotName]() { return GetCardBrush(ExpandedSlot == SlotName); })
                .Padding(11.0f)
                .HAlign(HAlign_Center)
                [
                    SNew(SBBBCharacterCustomizationIcon)
                    .Symbol(SlotName)
                    .Size(25.0f)
                    .Color_Lambda([this, SlotName]() { return ExpandedSlot == SlotName ? Accent : Ink; })
                ]
            ]
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
        return MakeIcon(TEXT("Warning"), 32.0f, Accent);
    }
    const FSlateBrush* Thumbnail = GetItemBrush(ItemId);
    const bool bEmptyItem = Item.Mesh.IsNull() && Item.Attachments.IsEmpty();
    const bool bUnconfiguredAttachment = PartSlot == TEXT("Attachments") && bEmptyItem
        && ItemId != TEXT("Attachments_None");
    if (bUnconfiguredAttachment)
    {
        UE_LOG(LogBBBCustomizationCards, Error, TEXT("附件组合没有挂载配置 Item=%s"), *ItemId.ToString());
    }
    return SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .IsEnabled(!bUnconfiguredAttachment)
        .ToolTipText(Item.DisplayName)
        .ContentPadding(0.0f)
        .OnHovered_Lambda([this, PartSlot, ItemId]()
        {
            HoveredPartSlot = PartSlot;
            HoveredItemId = ItemId;
        })
        .OnUnhovered_Lambda([this, ItemId]()
        {
            if (HoveredItemId == ItemId)
            {
                HoveredPartSlot = NAME_None;
                HoveredItemId = NAME_None;
            }
        })
        .OnClicked_Lambda([this, PartSlot, ItemId]()
        {
            if (!Session || !Session->SelectItem(PartSlot, ItemId))
            {
                UE_LOG(LogBBBCustomizationCards, Warning, TEXT("款式预览失败 Slot=%s Item=%s"),
                    *PartSlot.ToString(), *ItemId.ToString());
                StatusMessage = FText::FromString(TEXT("预览失败  保留原选择"));
                return FReply::Handled();
            }
            RefreshSlotCardThumbnails();
            StatusMessage = FText::FromString(TEXT("预览中  应用后保存"));
            InvalidateLayoutAndVolatility();
            return FReply::Handled();
        })
        [
            SNew(SBorder)
            .BorderImage_Lambda([this, PartSlot, ItemId]()
            {
                return GetCardBrush(GetSelectedItemId(PartSlot) == ItemId, HoveredItemId == ItemId);
            })
            .Padding(1.0f)
            [
                SNew(SBox)
                .HeightOverride(168.0f)
                [
                    SNew(SOverlay)
                    + SOverlay::Slot()
                    .Padding(9.0f, 9.0f, 9.0f, 31.0f)
                    [
                        Thumbnail
                            ? StaticCastSharedRef<SWidget>(SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
                                [SNew(SImage).Image(Thumbnail)])
                            : StaticCastSharedRef<SWidget>(SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                                [MakeIcon(bEmptyItem && !bUnconfiguredAttachment ? TEXT("Empty") : TEXT("Warning"),
                                    38.0f, MutedInk)])
                    ]
                    + SOverlay::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Top)
                    .Padding(8.0f)
                    [
                        SNew(SBBBCharacterCustomizationIcon)
                        .Symbol(TEXT("Apply"))
                        .Size(16.0f)
                        .Color(Accent)
                        .Visibility_Lambda([this, PartSlot, ItemId]()
                        {
                            return GetSelectedItemId(PartSlot) == ItemId
                                ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
                        })
                    ]
                    + SOverlay::Slot()
                    .VAlign(VAlign_Bottom)
                    .Padding(8.0f, 0.0f, 8.0f, 8.0f)
                    [
                        SNew(STextBlock)
                        .Text(Item.DisplayName)
                        .Font(GetCustomizationFont(11))
                        .ColorAndOpacity(Ink)
                        .OverflowPolicy(ETextOverflowPolicy::Ellipsis)
                        .Visibility(EVisibility::HitTestInvisible)
                    ]
                ]
            ]
        ];
}

TSharedRef<SWidget> UBBBCharacterCustomizationView::MakeItemGrid(const FName PartSlot)
{
    const TArray<FName> Items = Session->GetItems(PartSlot);
    TSharedRef<SVerticalBox> Panel = SNew(SVerticalBox);
    Panel->AddSlot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 16.0f)
    [
        MakeSlotStrip(IsLeftSideSlot(PartSlot))
    ];
    TSharedRef<SHorizontalBox> Header = SNew(SHorizontalBox);
    Header->AddSlot().AutoWidth().VAlign(VAlign_Center)
    [
        SNew(SButton)
        .ButtonStyle(&ActionButtonStyle)
        .ToolTipText(FText::FromString(TEXT("返回穿戴部位")))
        .ContentPadding(10.0f)
        .OnClicked_Lambda([this]()
        {
            CloseSlot();
            return FReply::Handled();
        })
        [
            MakeIcon(TEXT("Back"), 22.0f)
        ]
    ];
    Header->AddSlot().FillWidth(1.0f).VAlign(VAlign_Center).Padding(10.0f, 0.0f)
    [
        SNew(STextBlock)
        .Text(GetPartSlotTitle(PartSlot))
        .Font(GetCustomizationFont(17))
        .ColorAndOpacity(Ink)
    ];
    Header->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(8.0f, 0.0f)
    [
        SNew(STextBlock)
        .Text(FText::AsNumber(Items.Num()))
        .Font(GetCustomizationFont(12))
        .ColorAndOpacity(MutedInk)
    ];
    if (PartSlot == TEXT("Body") || PartSlot == TEXT("Vest"))
    {
        Header->AddSlot().AutoWidth().VAlign(VAlign_Center)
        [
            SNew(SButton)
            .ButtonStyle(&ActionButtonStyle)
            .ToolTipText(FText::FromString(TEXT("选择徽章图案")))
            .ContentPadding(10.0f)
            .OnClicked_Lambda([this, PartSlot]()
            {
                ShowPatchGrid(PartSlot);
                return FReply::Handled();
            })
            [
                MakeIcon(TEXT("Patch"), 24.0f)
            ]
        ];
    }
    Panel->AddSlot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 8.0f)[Header];

    TSharedRef<SUniformGridPanel> Grid = SNew(SUniformGridPanel).SlotPadding(FMargin(5.0f));
    for (int32 Index = 0; Index < Items.Num(); ++Index)
    {
        Grid->AddSlot(Index % 3, Index / 3)[MakeItemCard(PartSlot, Items[Index])];
    }
    Panel->AddSlot().FillHeight(1.0f)
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
    Panel->AddSlot().AutoHeight().Padding(6.0f, 12.0f)
    [
        SNew(STextBlock)
        .Text_Lambda([this, PartSlot]()
        {
            return GetItemDisplayName(HoveredItemId.IsNone() ? GetSelectedItemId(PartSlot) : HoveredItemId);
        })
        .Font(GetCustomizationFont(13))
        .ColorAndOpacity(Ink)
        .AutoWrapText(true)
    ];
    return Panel;
}
