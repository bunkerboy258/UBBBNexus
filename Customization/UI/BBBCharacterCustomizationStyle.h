#pragma once

#include "CoreMinimal.h"
#include "Styling/CoreStyle.h"
#include "Fonts/CompositeFont.h"
#include "Misc/Paths.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "SBBBCharacterCustomizationIcon.h"
#include "Widgets/Layout/SBox.h"
#include "Engine/FontFace.h"
#include "UObject/StrongObjectPtr.h"

namespace BBBCustomizationStyle
{
    inline const FLinearColor Accent(0.92f, 0.49f, 0.16f);
    inline const FLinearColor Ink(0.78f, 0.80f, 0.79f);
    inline const FLinearColor MutedInk(0.38f, 0.43f, 0.47f);

    /** @return 随界面状态选取的细边卡片画刷 */
    inline const FSlateBrush* GetCardBrush(bool bSelected = false, bool bHovered = false)
    {
        static const FSlateRoundedBoxBrush Normal(FLinearColor(0.009f, 0.013f, 0.018f, 0.78f), 1.0f,
            FLinearColor(0.22f, 0.26f, 0.29f, 0.65f), 1.0f);
        static const FSlateRoundedBoxBrush Hovered(FLinearColor(0.028f, 0.034f, 0.040f, 0.88f), 1.0f,
            FLinearColor(0.65f, 0.70f, 0.72f), 1.0f);
        static const FSlateRoundedBoxBrush Selected(FLinearColor(0.042f, 0.031f, 0.022f, 0.88f), 1.0f,
            Accent, 1.0f);
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
