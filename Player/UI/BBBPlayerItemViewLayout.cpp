#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Client/UI/BBBPlayerMenuStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SOverlay.h"

using namespace BBBCustomizationStyle;

TSharedRef<SWidget> UBBBPlayerItemView::RebuildWidget()
{
    TSharedRef<SWidget> Result = SNew(SOverlay)
        + SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom).Padding(28.0f, 0.0f, 64.0f, 54.0f)
        [
            SNew(SScaleBox).Stretch(EStretch::ScaleToFit).StretchDirection(EStretchDirection::DownOnly)
            .HAlign(HAlign_Right).VAlign(VAlign_Bottom)
            .Visibility_Lambda([this]()
            {
                const ABBBPlayerController *Controller = GetItemController();
                return Controller && Controller->HasItemInventory() && !Controller->IsPlayerMenuOpen()
                    ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
            })
            [MakeGameplayHud()]
        ]
        + SOverlay::Slot()
        [
            SNew(SBorder).BorderImage(BBBPlayerMenuStyle::GetSolidBrush())
            .BorderBackgroundColor(FLinearColor(0.009f, 0.012f, 0.020f, 0.985f)).Padding(0.0f)
            .Visibility_Lambda([this]() { return bBackpackOpen ? EVisibility::Visible : EVisibility::Collapsed; })
            [
                SNew(SScaleBox).Stretch(EStretch::ScaleToFit)
                [SNew(SBox).WidthOverride(1920.0f).HeightOverride(1080.0f)[MakeBackpack()]]
            ]
        ];
    RefreshItems();
    return Result;
}

TSharedRef<SWidget> UBBBPlayerItemView::MakeBackpack()
{
    TSharedRef<SConstraintCanvas> Layout = SNew(SConstraintCanvas);
    Layout->AddSlot().Anchors(FAnchors(0.065f, 0.055f, 0.935f, 0.16f)).Offset(FMargin(0.0f))
    [
        BBBPlayerMenuStyle::MakeNavigation(true, FOnClicked::CreateWeakLambda(this, [this]()
        {
            GetItemController()->ToggleCustomization();
            return FReply::Handled();
        }), FOnClicked())
    ];
    Layout->AddSlot().Anchors(FAnchors(0.70f, 0.072f, 0.935f, 0.14f)).Offset(FMargin(0.0f))
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right)
        [
            SNew(STextBlock).Text_Lambda([this]() { return GetCapacityText(); })
            .Font(GetCustomizationFont(20)).ColorAndOpacity(Ink)
        ]
        + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(0.0f, 6.0f)
        [
            SNew(STextBlock).Text(FText::FromString(TEXT("随身物品与快捷装备")))
            .Font(GetCustomizationFont(11)).ColorAndOpacity(MutedInk)
        ]
    ];
    static const FScrollBarStyle ScrollBar = FScrollBarStyle()
        .SetNormalThumbImage(FSlateColorBrush(FLinearColor(0.26f, 0.24f, 0.20f, 0.7f)))
        .SetHoveredThumbImage(FSlateColorBrush(Accent)).SetDraggedThumbImage(FSlateColorBrush(Accent));
    static const FScrollBoxStyle ScrollBox = FScrollBoxStyle()
        .SetTopShadowBrush(FSlateNoResource()).SetBottomShadowBrush(FSlateNoResource());
    Layout->AddSlot().Anchors(FAnchors(0.0625f, 0.215f, 0.645f, 0.88f)).Offset(FMargin(0.0f))
    [
        SNew(SScrollBox).Style(&ScrollBox).ScrollBarStyle(&ScrollBar)
        + SScrollBox::Slot().Padding(0.0f, 0.0f, 8.0f, 0.0f)
        [SAssignNew(BackpackSlots, SVerticalBox)]
    ];
    Layout->AddSlot().Anchors(FAnchors(0.70f, 0.215f, 0.935f, 0.875f)).Offset(FMargin(0.0f))
    [MakeDetails()];
    Layout->AddSlot().Anchors(FAnchors(0.065f, 0.92f, 0.935f, 0.975f)).Offset(FMargin(0.0f))
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
        [
            SNew(SButton).ButtonStyle(&BBBPlayerMenuStyle::GetButtonStyle()).ContentPadding(FMargin(14.0f, 9.0f))
            .OnClicked_Lambda([this]() { GetItemController()->ToggleBackpack(); return FReply::Handled(); })
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[MakeIcon(TEXT("Back"), 24.0f)]
                + SHorizontalBox::Slot().AutoWidth().Padding(12.0f, 0.0f)
                [SNew(STextBlock).Text(FText::FromString(TEXT("返回游玩  [Esc]")))
                    .Font(GetCustomizationFont(13)).ColorAndOpacity(Ink)]
            ]
        ]
        + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center).Padding(30.0f, 0.0f)
        [
            SAssignNew(StatusText, STextBlock)
            .Text(FText::FromString(TEXT("拖动整理  ·  点击快捷槽位装备")))
            .Font(GetCustomizationFont(12)).ColorAndOpacity(MutedInk)
        ]
        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
        [
            SNew(STextBlock).Text(FText::FromString(TEXT("1–5  快捷装备     F6  换装     Tab  关闭")))
            .Font(GetCustomizationFont(12)).ColorAndOpacity(Ink)
        ]
    ];
    return Layout;
}

TSharedRef<SWidget> UBBBPlayerItemView::MakeDetails()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight()
        [
            SNew(STextBlock).Text(FText::FromString(TEXT("物品详情")))
            .Font(GetCustomizationFont(14)).ColorAndOpacity(Ink)
        ]
        + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 17.0f, 0.0f, 24.0f)
        [
            SNew(SBorder).BorderImage(GetCardBrush()).Padding(1.0f)
            [
                SNew(SBox).HeightOverride(192.0f)
                [
                    SNew(SOverlay)
                    + SOverlay::Slot()
                    [SNew(SImage).Image(&CardSurfaceBrush)
                        .ColorAndOpacity(FLinearColor(0.70f, 0.67f, 0.60f, 0.58f)).Visibility(EVisibility::HitTestInvisible)]
                    + SOverlay::Slot()
                    [SNew(SImage).Image(&CardFrameBrush)
                        .ColorAndOpacity(FLinearColor(0.60f, 0.58f, 0.51f, 0.32f)).Visibility(EVisibility::HitTestInvisible)]
                    + SOverlay::Slot().Padding(28.0f)
                    [SAssignNew(DetailArtwork, SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)]
                ]
            ]
        ]
        + SVerticalBox::Slot().AutoHeight()
        [SAssignNew(DetailName, STextBlock).Font(GetCustomizationFont(23)).ColorAndOpacity(Ink).AutoWrapText(true)]
        + SVerticalBox::Slot().FillHeight(1.0f).Padding(0.0f, 18.0f, 0.0f, 20.0f)
        [
            SAssignNew(DetailDescription, STextBlock).Font(GetCustomizationFont(13))
            .ColorAndOpacity(Ink).AutoWrapText(true)
        ]
        + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 16.0f)
        [
            SNew(STextBlock).Text_Lambda([this]() { return GetActiveItemText(); })
            .Font(GetCustomizationFont(13)).ColorAndOpacity(Accent).AutoWrapText(true)
        ]
        + SVerticalBox::Slot().AutoHeight()
        [
            SNew(SButton).ButtonStyle(&BBBPlayerMenuStyle::GetButtonStyle()).ContentPadding(12.0f)
            .OnClicked_Lambda([this]() { SelectSlot(INDEX_NONE); return FReply::Handled(); })
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("收起手持装备")))
                .Font(GetCustomizationFont(14)).ColorAndOpacity(Ink)
            ]
        ]
        + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 12.0f, 0.0f, 0.0f)
        [
            SNew(STextBlock).Text(FText::FromString(TEXT("右键快捷格也可收起装备\n携带物品需先移至快捷格，才能装备")))
            .Font(GetCustomizationFont(11)).ColorAndOpacity(MutedInk).AutoWrapText(true)
        ];
}
