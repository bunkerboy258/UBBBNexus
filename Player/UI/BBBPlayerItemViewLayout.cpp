#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationStyle.h"
#include "Brushes/SlateColorBrush.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

TSharedRef<SWidget> UBBBPlayerItemView::RebuildWidget()
{
    TSharedRef<SWidget> Result = SNew(SOverlay)
        + SOverlay::Slot()[MakeGameplayHud()]
        + SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Top).Padding(12.0f, 76.0f)
        [
            SNew(SBorder).Visibility(EVisibility::HitTestInvisible)
            .BorderImage(FCoreStyle::Get().GetBrush("NoBrush")).Padding(0.0f)
            .ColorAndOpacity_Lambda([this]() { return FLinearColor(1.0f, 1.0f, 1.0f, GetQuickSelectionOpacity()); })
            [SAssignNew(QuickBar, SHorizontalBox)]
        ]
        + SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Center).Padding(20.0f, 24.0f, 32.0f, 140.0f)
        [
            SNew(SScaleBox).Stretch(EStretch::ScaleToFit).StretchDirection(EStretchDirection::DownOnly)
            .HAlign(HAlign_Right)
            .Visibility_Lambda([this]() { return bBackpackOpen ? EVisibility::Visible : EVisibility::Collapsed; })
            [SNew(SBox).WidthOverride(460.0f).HeightOverride(760.0f)[MakeBackpack()]]
        ];
    RefreshItems();
    return Result;
}

TSharedRef<SWidget> UBBBPlayerItemView::MakeBackpack()
{
    static const FSlateColorBrush Background(FLinearColor::White);
    return SNew(SBorder).BorderImage(&Background)
        .BorderBackgroundColor(FLinearColor(0.012f, 0.012f, 0.012f, 0.82f)).Padding(22.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 22.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1.0f)
                [SNew(STextBlock).Text(FText::FromString(TEXT("背包")))
                    .Font(BBBCustomizationStyle::GetCustomizationFont(22)).ColorAndOpacity(FLinearColor::White)]
                + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                [SNew(STextBlock).Text_Lambda([this]() { return GetCapacityText(); })
                    .Font(BBBCustomizationStyle::GetCustomizationFont(11))
                    .ColorAndOpacity(FLinearColor(0.65f, 0.65f, 0.65f))]
            ]
            + SVerticalBox::Slot().FillHeight(1.0f)
            [
                SNew(SScrollBox)
                + SScrollBox::Slot()[SAssignNew(BackpackSlots, SVerticalBox)]
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 16.0f, 0.0f, 0.0f)
            [
                SAssignNew(StatusText, STextBlock).Text(FText::GetEmpty())
                .Font(BBBCustomizationStyle::GetCustomizationFont(10))
                .ColorAndOpacity(FLinearColor(0.75f, 0.75f, 0.75f)).AutoWrapText(true)
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 12.0f, 0.0f, 0.0f)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("拖动整理                         TAB  关闭")))
                .Font(BBBCustomizationStyle::GetCustomizationFont(10))
                .ColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f))
            ]
        ];
}
