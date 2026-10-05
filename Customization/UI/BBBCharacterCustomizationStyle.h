#pragma once

#include "CoreMinimal.h"
#include "Styling/CoreStyle.h"
#include "Fonts/CompositeFont.h"
#include "Misc/Paths.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Brushes/SlateColorBrush.h"
#include "SBBBCharacterCustomizationIcon.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Text/STextBlock.h"
#include "Engine/FontFace.h"
#include "UObject/StrongObjectPtr.h"

namespace BBBCustomizationStyle
{
    inline const FLinearColor Accent(0.72f, 0.43f, 0.17f);
    inline const FLinearColor Ink(0.79f, 0.77f, 0.69f);
    inline const FLinearColor MutedInk(0.36f, 0.36f, 0.33f);

    /** @return 随界面状态选取的细边卡片画刷 */
    inline const FSlateBrush* GetCardBrush(bool bSelected = false, bool bHovered = false)
    {
        static const FSlateRoundedBoxBrush Normal(FLinearColor(0.003f, 0.004f, 0.004f, 0.24f), 0.0f,
            FLinearColor(0.26f, 0.27f, 0.25f, 0.30f), 0.65f);
        static const FSlateRoundedBoxBrush Hovered(FLinearColor(0.034f, 0.030f, 0.021f, 0.42f), 0.0f,
            FLinearColor(0.76f, 0.72f, 0.61f, 0.85f), 1.0f);
        static const FSlateRoundedBoxBrush Selected(FLinearColor(0.080f, 0.046f, 0.014f, 0.34f), 0.0f,
            Accent, 1.2f);
        if (bSelected)
        {
            return &Selected;
        }
        if (bHovered)
        {
            return &Hovered;
        }
        return &Normal;
    }

    /** @return 不参与命中的换装图标 */
    inline TSharedRef<SWidget> MakeIcon(FName Symbol, float Size = 24.0f, FLinearColor Color = Ink)
    {
        return SNew(SBBBCharacterCustomizationIcon)
            .Symbol(Symbol).Size(Size).Color(Color)
            .Visibility(EVisibility::HitTestInvisible);
    }

    inline FText GetPartSlotTitle(const FName PartSlot)
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

    inline TArray<FName> GetPartSlotOrder(const bool bLeft)
    {
        if (bLeft)
        {
            return {TEXT("Head"), TEXT("Helmet"), TEXT("Body"), TEXT("Arms"), TEXT("Vest")};
        }

        return {TEXT("Backpack"), TEXT("Belt"), TEXT("Legs"), TEXT("Boots"), TEXT("Attachments")};
    }

    inline FText GetViewTitle(const FName ViewName)
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

    inline FSlateFontInfo GetCustomizationFont(const int32 Size)
    {
        // 游戏界面直接引用中文字体 避免编辑器主题改写全局复合字体
        static const TStrongObjectPtr<UFontFace> Face(LoadObject<UFontFace>(nullptr,
            TEXT("/Game/_Project/Customization/UI/Fonts/NotoSansCJKsc_Regular.NotoSansCJKsc_Regular")));
        static const TSharedPtr<const FCompositeFont> Font = []()
        {
            checkf(Face.IsValid(), TEXT("换装中文字体资产缺失"));
            TSharedPtr<FCompositeFont> Result = MakeShared<FCompositeFont>();
            FTypefaceEntry& Entry = Result->DefaultTypeface.Fonts.AddDefaulted_GetRef();
            Entry.Font = FFontData(Face.Get());
            return Result;
        }();
        return FSlateFontInfo(Font, Size);
    }

    /**
     * @param Text	当前控件的简短说明
     * @return 与装备卡面一致的悬浮说明
     */
    inline TSharedRef<SToolTip> MakeTooltip(TAttribute<FText> Text)
    {
        return SNew(SToolTip)
            .BorderImage(FCoreStyle::Get().GetBrush("NoBrush"))
            [
                SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(FLinearColor(0.008f, 0.009f, 0.009f, 0.97f))
                .Padding(12.0f, 8.0f)
                [
                    SNew(STextBlock).Text(Text).Font(GetCustomizationFont(11)).ColorAndOpacity(Ink)
                ]
            ];
    }

    /** @return 不受编辑器主题影响的细线滑杆样式 */
    inline const FSliderStyle& GetSurfaceSliderStyle()
    {
        static const FSliderStyle Style = []()
        {
            FSlateColorBrush Bar(FLinearColor::White);
            FSlateRoundedBoxBrush Thumb(FLinearColor::White, 3.5f);
            Thumb.ImageSize = FVector2D(7.0f, 7.0f);
            return FSliderStyle()
                .SetNormalBarImage(Bar)
                .SetHoveredBarImage(Bar)
                .SetDisabledBarImage(Bar)
                .SetNormalThumbImage(Thumb)
                .SetHoveredThumbImage(Thumb)
                .SetDisabledThumbImage(Thumb)
                .SetBarThickness(1.5f);
        }();
        return Style;
    }

    inline const FTextBlockStyle &GetCustomizationButtonTextStyle()
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
