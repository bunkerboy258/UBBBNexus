#pragma once
#include "CoreMinimal.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Fonts/CompositeFont.h"
#include "Materials/MaterialInterface.h"
#include "Styling/SlateTypes.h"
#include "UObject/StrongObjectPtr.h"

/** 玩家物品界面的素材包画刷与项目字体 */
namespace BBBItemUI
{
inline const FLinearColor Accent(0.92f, 0.49f, 0.035f);
inline const FLinearColor MutedText(1.0f, 1.0f, 1.0f, 0.72f);
/** @param Path 素材资产路径 @return 保持资源生命周期的画刷 */
inline const FSlateBrush *Brush(const TCHAR *Path)
{
    static TMap<FString, TSharedPtr<FSlateBrush>> Brushes;
    static TArray<TStrongObjectPtr<UTexture2D>> Textures;
    const FString Key(Path);
    if (!Brushes.Contains(Key))
    {
        UTexture2D *Texture = LoadObject<UTexture2D>(nullptr, Path);
        ensureMsgf(Texture, TEXT("背包素材缺失 %s"), Path);
        Textures.Emplace(Texture);
        TSharedPtr<FSlateBrush> Value = MakeShared<FSlateBrush>();
        Value->SetResourceObject(Texture);
        Value->DrawAs = ESlateBrushDrawType::Image;
        Value->ImageSize = Texture ? FVector2D(Texture->GetSizeX(), Texture->GetSizeY()) : FVector2D(1.0);
        Brushes.Add(Key, Value);
    }
    return Brushes[Key].Get();
}
/** @return P08 普通物品格子 */
inline const FSlateBrush *Slot()
{
    return Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Item_Slot.T_Item_Slot"));
}
/** @return 使用素材包边缘绘制清晰的物品选择框 */
inline const FSlateBrush *Selection()
{
    static const FSlateBrush Frame = []()
    {
        FSlateBrush Result = *Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Attachment_Slot.T_Attachment_Slot"));
        Result.DrawAs = ESlateBrushDrawType::Border;
        Result.Margin = FMargin(2.0f / 858.0f, 2.0f / 394.0f);
        return Result;
    }();
    return &Frame;
}
/** @param Path	明确字体资产 @return 原稿字形与显式中文字体 */
inline TSharedPtr<const FCompositeFont> Typeface(const TCHAR *Path)
{
    static TMap<FString, TSharedPtr<const FCompositeFont>> Fonts;
    static TArray<TStrongObjectPtr<UFontFace>> Faces;
    const FString Key(Path);
    if (!Fonts.Contains(Key))
    {
        UFontFace *Face = LoadObject<UFontFace>(nullptr, Path);
        UFontFace *Chinese = LoadObject<UFontFace>(nullptr,
            TEXT("/Game/_Project/Customization/UI/Fonts/NotoSansCJKsc_Regular.NotoSansCJKsc_Regular"));
        checkf(Face && Chinese, TEXT("E01 界面字体缺失 禁止回退到默认字体 %s"), Path);
        Faces.Emplace(Face);
        Faces.Emplace(Chinese);
        TSharedPtr<FCompositeFont> Composite = MakeShared<FCompositeFont>();
        Composite->DefaultTypeface.Fonts.AddDefaulted_GetRef().Font = FFontData(Face);
        FCompositeSubFont &CJK = Composite->SubTypefaces.AddDefaulted_GetRef();
        CJK.ScalingFactor = 0.78f;
        CJK.CharacterRanges.Add(FInt32Range(0x2E80, 0xA000));
        CJK.CharacterRanges.Add(FInt32Range(0xF900, 0xFB00));
        CJK.CharacterRanges.Add(FInt32Range(0xFF00, 0xFFF0));
        CJK.Typeface.Fonts.AddDefaulted_GetRef().Font = FFontData(Chinese);
        Fonts.Add(Key, Composite);
    }
    return Fonts[Key];
}

/** @param Info 明确字形与字号 @return 使用 E01 旧化纹理的白色褪色字形 */
inline FSlateFontInfo Faded(FSlateFontInfo Info)
{
    static TStrongObjectPtr<UMaterialInterface> Material(LoadObject<UMaterialInterface>(nullptr,
        TEXT("/Game/_Project/UI/Bag/E01/M_E01_FadedWhiteFont.M_E01_FadedWhiteFont")));
    checkf(Material.IsValid(), TEXT("E01 褪色字体材质缺失"));
    Info.FontMaterial = Material.Get();
    return Info;
}

/** @param Size 字号 @return 项目字体 避免原生控件默认字体 */
inline FSlateFontInfo Font(int32 Size)
{
    return Faded(FSlateFontInfo(Typeface(TEXT("/Game/_ThirdParty/Fonts/BebasNeue/BebasNeue_Bold.BebasNeue_Bold")), Size));
}

/** @param Size 字号 @return 原稿轻字重大标题 */
inline FSlateFontInfo TitleFont(int32 Size)
{
    return Faded(FSlateFontInfo(Typeface(TEXT("/Game/_ThirdParty/Fonts/Akrobat/Akrobat_Bold.Akrobat_Bold")), Size));
}

/** @param Size 字号 @return P08 使用的艺术字字体 */
inline FSlateFontInfo ArtFont(int32 Size)
{
    return Faded(FSlateFontInfo(Typeface(TEXT("/Game/_ThirdParty/Fonts/BebasNeue/BebasNeue_Bold.BebasNeue_Bold")), Size));
}

/** @return 透明文字导航 保留素材包三角选中标识 */
inline const FButtonStyle *Navigation()
{
    static const FButtonStyle Style = []()
    {
        FSlateBrush Empty;
        Empty.DrawAs = ESlateBrushDrawType::NoDrawType;
        FButtonStyle Result;
        Result.SetNormal(Empty).SetHovered(Empty).SetPressed(Empty);
        Result.SetNormalForeground(MutedText).SetHoveredForeground(FLinearColor::White).SetPressedForeground(FLinearColor::White);
        Result.SetNormalPadding(FMargin(0.0f)).SetPressedPadding(FMargin(0.0f));
        return Result;
    }();
    return &Style;
}

/** @return 从原稿独立导出的旧化主按钮 */
inline const FButtonStyle *ExitButton()
{
    static const FButtonStyle Style = []()
    {
        const auto *Surface = Brush(TEXT("/Game/_Project/UI/Bag/E01/T_E01_ExitSurface.T_E01_ExitSurface"));
        FSlateBrush Normal = *Surface;
        Normal.TintColor = FLinearColor(0.24f, 0.24f, 0.24f);
        FSlateBrush Hover = *Surface;
        Hover.TintColor = FLinearColor(0.35f, 0.35f, 0.35f);
        FSlateBrush Pressed = *Surface;
        Pressed.TintColor = FLinearColor(0.16f, 0.16f, 0.16f);
        FButtonStyle Result;
        Result.SetNormal(Normal).SetHovered(Hover).SetPressed(Pressed);
        Result.SetNormalPadding(FMargin(16.0f, 9.0f)).SetPressedPadding(FMargin(16.0f, 10.0f));
        return Result;
    }();
    return &Style;
}

}
