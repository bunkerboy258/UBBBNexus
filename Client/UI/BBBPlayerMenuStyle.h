#pragma once

#include "BBBWork/UBBBNexus/Customization/UI/BBBCharacterCustomizationStyle.h"
#include "Brushes/SlateNoResource.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"

/** 换装与背包共用的页面导航 不持有页面或领域状态 */
namespace BBBPlayerMenuStyle
{
    /** @return 仅用于自绘几何色块的画刷 */
    inline const FSlateBrush *GetSolidBrush()
    {
        static const FSlateColorBrush Brush(FLinearColor::White);
        return &Brush;
    }
    inline const FButtonStyle &GetButtonStyle()
    {
        static const FButtonStyle Style = FButtonStyle()
            .SetNormal(FSlateNoResource())
            .SetHovered(FSlateColorBrush(FLinearColor(0.45f, 0.39f, 0.26f, 0.08f)))
            .SetPressed(FSlateColorBrush(FLinearColor(0.60f, 0.36f, 0.12f, 0.14f)))
            .SetNormalPadding(FMargin(0.0f)).SetPressedPadding(FMargin(0.0f));
        return Style;
    }

    /** @return 当前页面的同级导航 操作由调用者转交控制器 */
    inline TSharedRef<SWidget> MakeNavigation(bool bBackpack, FOnClicked Appearance, FOnClicked Backpack)
    {
        using namespace BBBCustomizationStyle;
        TSharedRef<SHorizontalBox> Tabs = SNew(SHorizontalBox);
        for (int32 Index = 0; Index < 2; ++Index)
        {
            const bool bSelected = bBackpack == (Index == 1);
            Tabs->AddSlot().AutoWidth().Padding(0.0f, 0.0f, 36.0f, 0.0f)
            [
                SNew(SButton).ButtonStyle(&GetButtonStyle()).ContentPadding(0.0f)
                .OnClicked(bSelected ? FOnClicked::CreateLambda([]() { return FReply::Handled(); })
                    : (Index == 0 ? Appearance : Backpack))
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight().Padding(2.0f, 9.0f, 20.0f, 13.0f)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                        [MakeIcon(Index == 0 ? TEXT("Vest") : TEXT("Backpack"), 26.0f, bSelected ? Accent : Ink)]
                        + SHorizontalBox::Slot().AutoWidth().Padding(12.0f, 0.0f)
                        [
                            SNew(STextBlock).Text(FText::FromString(Index == 0 ? TEXT("换装") : TEXT("背包")))
                            .Font(GetCustomizationFont(21)).ColorAndOpacity(bSelected ? Ink : MutedInk)
                        ]
                    ]
                    + SVerticalBox::Slot().AutoHeight()
                    [
                        SNew(SBox).HeightOverride(2.0f)
                        [SNew(SImage).Image(GetSolidBrush())
                            .ColorAndOpacity(bSelected ? Accent : FLinearColor::Transparent)]
                    ]
                ]
            ];
        }
        return SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().Padding(2.0f, 0.0f, 0.0f, 8.0f)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("行动准备")))
                .Font(GetCustomizationFont(11)).ColorAndOpacity(MutedInk)
            ]
            + SVerticalBox::Slot().AutoHeight()[Tabs];
    }
}
