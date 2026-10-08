#pragma once
#include "CoreMinimal.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Fonts/CompositeFont.h"
#include "Styling/SlateTypes.h"
#include "UObject/StrongObjectPtr.h"

/** 玩家物品界面的素材包画刷与项目字体 */
namespace BBBItemUI
{
inline const FLinearColor Accent(1.0f, 0.65f, 0.05f);
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
/** @return P08 穿戴附件格子 */
inline const FSlateBrush *GearSlot()
{
    return Brush(
        TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_Attachment_Slot.T_Attachment_Slot"));
}
/** @param Size 字号 @return 项目字体 避免原生控件默认字体 */
inline FSlateFontInfo Font(int32 Size)
{
    static const TStrongObjectPtr<UFontFace> Face(LoadObject<UFontFace>(
        nullptr, TEXT("/Game/_Project/Customization/UI/Fonts/NotoSansCJKsc_Regular.NotoSansCJKsc_Regular")));
    static const TSharedPtr<const FCompositeFont> Composite = []()
    {
        TSharedPtr<FCompositeFont> Result = MakeShared<FCompositeFont>();
        FTypefaceEntry &Entry = Result->DefaultTypeface.Fonts.AddDefaulted_GetRef();
        Entry.Font = FFontData(Face.Get());
        return Result;
    }();
    return FSlateFontInfo(Composite, Size);
}
/** @param Size 字号 @return P08 使用的艺术字字体 */
inline FSlateFontInfo ArtFont(int32 Size)
{
    static const TStrongObjectPtr<UFontFace> Face(
        LoadObject<UFontFace>(nullptr, TEXT("/Game/_ThirdParty/Fonts/BebasNeue/BebasNeue_Regular.BebasNeue_Regular")));
    static const TSharedPtr<const FCompositeFont> Composite = []()
    {
        TSharedPtr<FCompositeFont> Result = MakeShared<FCompositeFont>();
        FTypefaceEntry &Entry = Result->DefaultTypeface.Fonts.AddDefaulted_GetRef();
        Entry.Font = FFontData(Face.Get());
        return Result;
    }();
    return FSlateFontInfo(Composite, Size);
}
/** @return 素材包按钮的三个交互状态 */
inline const FButtonStyle *Button()
{
    static const FButtonStyle Style = []()
    {
        const FSlateBrush *Normal =
            Brush(TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_EquipBtn.T_EquipBtn"));
        const FSlateBrush *Hover = Brush(
            TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/Weapon_Customization/T_EquippedBtn.T_EquippedBtn"));
        FButtonStyle Result;
        Result.SetNormal(*Normal).SetHovered(*Hover).SetPressed(*Hover);
        Result.SetNormalPadding(FMargin(8.0f)).SetPressedPadding(FMargin(8.0f));
        return Result;
    }();
    return &Style;
}
}
