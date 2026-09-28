#pragma once

#include "CoreMinimal.h"
#include "Styling/CoreStyle.h"
#include "Fonts/CompositeFont.h"
#include "Misc/Paths.h"

namespace BBBCustomizationStyle
{
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
        static const TSharedPtr<const FCompositeFont> Font = MakeShared<FCompositeFont>(
            NAME_None,
            FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"),
            EFontHinting::Default,
            EFontLoadingPolicy::LazyLoad);
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
