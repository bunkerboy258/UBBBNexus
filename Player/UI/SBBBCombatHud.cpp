#include "BBBWork/UBBBNexus/Player/UI/SBBBCombatHud.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBCombatAmmoRail.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "Rendering/DrawElementTypes.h"
#include "Brushes/SlateColorBrush.h"

void SBBBCombatHud::Construct(const FArguments &Arguments)
{
    View = Arguments._View;
    Crosshair.Reset(LoadObject<UTexture2D>(nullptr, TEXT("/Game/_ThirdParty/UI/CrosshairFreePack/Textures/T_CH001.T_CH001")));
    Heart.Reset(LoadObject<UTexture2D>(nullptr, TEXT("/Game/_ThirdParty/UI/EditableSurvivalHorrorUI/HUD/T_HUD_03.T_HUD_03")));
    SetVisibility(EVisibility::HitTestInvisible);
    ForceVolatile(true);
}

void SBBBCombatHud::Tick(const FGeometry &Geometry, double CurrentTime, float DeltaTime)
{
    SLeafWidget::Tick(Geometry, CurrentTime, DeltaTime);
    const ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    DamageAge += DeltaTime;
    EmptyAge += DeltaTime;
    NameAge += DeltaTime;
    float Health = 0.0f;
    float Maximum = 0.0f;
    uint8 Phase = 255;
    if (!Controller || !Controller->ReadHudHealth(Health, Maximum, Phase))
    {
        ObservedPawn.Reset();
        bLowHealth = false;
        DamageAge = 1.0f;
        return;
    }
    const float Fraction = Controller->GetHudHealthFraction();
    const bool bReset = ObservedPawn.Get() != Controller->GetPawn() || PreviousPhase != Phase
        || PreviousMaximum != Maximum;
    if (bReset)
    {
        DamageAge = 1.0f;
        DamageTrail = Fraction;
        bLowHealth = Phase == 0 && Fraction <= 0.30f;
    }
    if (!bReset && Health > PreviousHealth)
    {
        DamageAge = 1.0f;
        DamageTrail = Fraction;
    }
    if (!bReset && Phase == 0 && Health < PreviousHealth)
    {
        DamageTrail = FMath::Lerp(DamageTrail, PreviousHealth / FMath::Max(1.0f, Maximum),
            FMath::Clamp((DamageAge - DeltaTime - 0.15f) / 0.45f, 0.0f, 1.0f));
        DamageAge = 0.0f;
    }
    if (Phase != 0 || Fraction > 0.35f)
    {
        bLowHealth = false;
    }
    if (Phase == 0 && Fraction <= 0.30f)
    {
        bLowHealth = true;
    }
    ObservedPawn = Controller->GetPawn();
    PreviousHealth = Health;
    PreviousMaximum = Maximum;
    PreviousPhase = Phase;
    int32 Start = 0;
    int32 Count = 0;
    Controller->GetConsumableQuickRange(Start, Count);
    const int32 Selected = Controller->GetSelectedConsumableSlot();
    const auto Item = Controller->GetItemDisplayData(Selected >= 0 && Selected < Count ? Start + Selected : INDEX_NONE);
    if (bReset || Selected != ConsumableSlot || Item.InstanceId != ConsumableId)
    {
        NameAge = 0.0f;
    }
    ConsumableSlot = Selected;
    ConsumableId = Item.InstanceId;
    const auto *Equipment = Cast<ABBBEquipment>(Controller->GetActiveItem());
    int32 Loaded = 0;
    int32 Capacity = 0;
    bool Continuous = false;
    const bool bEmpty = Equipment && Equipment->ReadAmmoDisplay(Loaded, Capacity, Continuous)
        && Capacity > 0 && Loaded == 0 && !Equipment->IsReloading();
    if (bEmpty && (!bWasEmpty || ObservedEquipment.Get() != Equipment))
    {
        EmptyAge = 0.0f;
    }
    bWasEmpty = bEmpty;
    ObservedEquipment = const_cast<ABBBEquipment *>(Equipment);
}

FVector2D SBBBCombatHud::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return FVector2D(1.0f);
}

int32 SBBBCombatHud::OnPaint(const FPaintArgs &Args, const FGeometry &Geometry,
    const FSlateRect &CullingRect, FSlateWindowElementList &Elements, int32 Layer,
    const FWidgetStyle &Style, bool bParentEnabled) const
{
    const ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    if (!Controller || !Controller->HasItemInventory() || Controller->IsPlayerMenuOpen() || Controller->IsBackpackOpen())
    {
        return Layer;
    }
    const FVector2D Size = Geometry.GetLocalSize();
    const float Scale = FMath::Clamp(static_cast<float>(Size.Y / 1080.0), 0.55f, 1.6f);
    const FLinearColor White(1.0f, 1.0f, 1.0f, 0.95f);
    const auto Line = [&](const TArray<FVector2D> &Points, float Width, FLinearColor Color)
    {
        FSlateDrawElement::MakeLines(Elements, Layer + 1, Geometry.ToPaintGeometry(), Points,
            ESlateDrawEffect::None, FLinearColor(0.0f, 0.0f, 0.0f, Color.A * 0.55f), true, (Width + 2.0f) * Scale);
        FSlateDrawElement::MakeLines(Elements, Layer + 2, Geometry.ToPaintGeometry(), Points,
            ESlateDrawEffect::None, Color, true, Width * Scale);
    };
    const auto Arc = [&](FVector2D Center, float Radius, float From, float To, float Width, FLinearColor Color)
    {
        if (To <= From)
        {
            return;
        }
        TArray<FVector2D> Points;
        const int32 Steps = FMath::Max(2, FMath::CeilToInt((To - From) / 4.0f));
        for (int32 Step = 0; Step <= Steps; ++Step)
        {
            const float Angle = FMath::DegreesToRadians(FMath::Lerp(From, To, static_cast<float>(Step) / Steps));
            Points.Add(Center + FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius * Scale);
        }
        Line(Points, Width, Color);
    };

    const auto Text = [&](FVector2D Position, const FString &Value, int32 FontSize, FLinearColor Color)
    {
        FSlateDrawElement::MakeText(Elements, Layer + 3,
            Geometry.ToPaintGeometry(FVector2f(300.0f * Scale, 40.0f * Scale), FSlateLayoutTransform(FVector2f(Position))),
            Value, BBBItemUI::Font(FMath::RoundToInt(FontSize * Scale)), ESlateDrawEffect::None, Color);
    };
    const auto Image = [&](UTexture2D *Texture, FVector2D Position, FVector2D Extent, FLinearColor Color)
    {
        if (!Texture)
        {
            return;
        }
        FSlateBrush Brush;
        Brush.SetResourceObject(Texture);
        FSlateDrawElement::MakeBox(Elements, Layer + 2,
            Geometry.ToPaintGeometry(FVector2f(Extent), FSlateLayoutTransform(FVector2f(Position))),
            &Brush, ESlateDrawEffect::None, Color);
    };
    static const FSlateColorBrush Solid(FLinearColor::White);
    if (DamageAge < 0.35f)
    {
        const float Alpha = 0.13f * (1.0f - DamageAge / 0.35f);
        for (int32 Edge = 0; Edge < 12; ++Edge)
        {
            const float Inset = Edge * 3.0f * Scale;
            const FLinearColor Tint(0.55f, 0.015f, 0.01f, Alpha * (1.0f - Edge / 12.0f));
            const auto Strip = [&](FVector2D Position, FVector2D Extent)
            {
                FSlateDrawElement::MakeBox(Elements, Layer, Geometry.ToPaintGeometry(FVector2f(Extent),
                    FSlateLayoutTransform(FVector2f(Position))), &Solid, ESlateDrawEffect::None, Tint);
            };
            Strip(FVector2D(Inset, Inset), FVector2D(Size.X - Inset * 2.0f, 3.0f * Scale));
            Strip(FVector2D(Inset, Size.Y - Inset - 3.0f * Scale), FVector2D(Size.X - Inset * 2.0f, 3.0f * Scale));
            Strip(FVector2D(Inset, Inset), FVector2D(3.0f * Scale, Size.Y - Inset * 2.0f));
            Strip(FVector2D(Size.X - Inset - 3.0f * Scale, Inset), FVector2D(3.0f * Scale, Size.Y - Inset * 2.0f));
        }
    }
    const float Health = Controller->GetHudHealthFraction();
    const double Half = 180.0 * Scale;
    const double Y = Size.Y - 38.0 * Scale;
    Line({FVector2D(Size.X * 0.5 - Half, Y), FVector2D(Size.X * 0.5 + Half, Y)},
        1.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.16f));
    if (DamageAge < 0.60f)
    {
        const float Trail = FMath::Lerp(DamageTrail, Health, FMath::Clamp((DamageAge - 0.15f) / 0.45f, 0.0f, 1.0f));
        Line({FVector2D(Size.X * 0.5 - Half * Trail, Y), FVector2D(Size.X * 0.5 + Half * Trail, Y)},
            3.0f, FLinearColor(0.65f, 0.20f, 0.12f, 0.7f));
    }
    if (bLowHealth)
    {
        Image(Heart.Get(), FVector2D(Size.X * 0.5 - Half - 30.0f * Scale, Y - 11.0f * Scale),
            FVector2D(22.0f * Scale), FLinearColor(0.65f, 0.08f, 0.06f));
    }
    if (Health > 0.0f)
    {
        Line({FVector2D(Size.X * 0.5 - Half * Health, Y), FVector2D(Size.X * 0.5 + Half * Health, Y)},
            2.0f, bLowHealth ? FLinearColor(0.65f, 0.08f, 0.06f) : White);
    }

    const ABBBEquipment *Equipment = Cast<ABBBEquipment>(Controller->GetActiveItem());
    const UBBBEquipmentDefinition *Definition = IsValid(Equipment) ? Equipment->GetDefinition() : nullptr;
    if (Definition && Definition->Icon)
    {
        const UTexture2D *Texture = Definition->Icon;
        const double Width = 160.0 * Scale;
        const double Height = Width * Texture->GetSizeY() / FMath::Max(1, Texture->GetSizeX());
        FSlateBrush Brush;
        Brush.SetResourceObject(Definition->Icon);
        Brush.ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
        const FVector2D Position(Size.X - Width - 42.0 * Scale, Size.Y - Height - 65.0 * Scale);
        FSlateDrawElement::MakeBox(Elements, Layer + 2,
            Geometry.ToPaintGeometry(FVector2f(Width, Height), FSlateLayoutTransform(FVector2f(Position))),
            &Brush, ESlateDrawEffect::None, White);
    }
    int32 Loaded = 0;
    int32 Capacity = 0;
    bool bContinuous = false;
    const bool bHasAmmoDisplay = Equipment && Equipment->ReadAmmoDisplay(Loaded, Capacity, bContinuous) && Capacity > 0;
    FLinearColor AmmoColor = White;
    if (bHasAmmoDisplay)
    {
        Loaded = FMath::Clamp(Loaded, 0, Capacity);
        const float AmmoLeft = Size.X - 184.0f * Scale;
        const float AmmoTop = Size.Y - 62.0f * Scale;
        AmmoColor = Loaded <= FMath::Max(1, Capacity / 4)
            ? BBBItemUI::Accent : White;

        Line({FVector2D(AmmoLeft - 11.0f * Scale, AmmoTop + 4.0f * Scale),
            FVector2D(AmmoLeft - 11.0f * Scale, AmmoTop + 30.0f * Scale)}, 2.0f, BBBItemUI::Accent);
        FSlateDrawElement::MakeText(Elements, Layer + 3,
            Geometry.ToPaintGeometry(FVector2f(70.0f * Scale, 35.0f * Scale),
                FSlateLayoutTransform(FVector2f(AmmoLeft, AmmoTop))),
            FString::FromInt(Loaded), BBBItemUI::Font(FMath::RoundToInt(27.0f * Scale)),
            ESlateDrawEffect::None, AmmoColor);
        FSlateDrawElement::MakeText(Elements, Layer + 3,
            Geometry.ToPaintGeometry(FVector2f(70.0f * Scale, 23.0f * Scale),
                FSlateLayoutTransform(FVector2f(AmmoLeft + 69.0f * Scale, AmmoTop + 10.0f * Scale))),
            FString::Printf(TEXT("/ %d"), Capacity), BBBItemUI::Font(FMath::RoundToInt(15.0f * Scale)),
            ESlateDrawEffect::None, FLinearColor(1.0f, 1.0f, 1.0f, 0.70f));
    }

    if (bHasAmmoDisplay && (Equipment->IsReloading() || Loaded == 0))
    {
        const float Flash = Loaded == 0 && !Equipment->IsReloading() && EmptyAge < 0.3f
            ? 0.65f + 0.35f * FMath::Abs(FMath::Cos(EmptyAge * PI / 0.3f)) : 1.0f;
        Text(FVector2D(Size.X - 184.0f * Scale, Size.Y - 26.0f * Scale),
            Equipment->IsReloading() ? TEXT("换弹中") : TEXT("空弹匣"), 16, BBBItemUI::Accent.CopyWithNewOpacity(Flash));
    }
    int32 Start = 0;
    int32 Count = 0;
    Controller->GetConsumableQuickRange(Start, Count);
    const int32 Selected = Controller->GetSelectedConsumableSlot();
    const auto Item = Controller->GetItemDisplayData(Selected >= 0 && Selected < Count ? Start + Selected : INDEX_NONE);
    const FVector2D ItemPosition(Size.X - 330.0f * Scale, Size.Y - 97.0f * Scale);
    FSlateDrawElement::MakeBox(Elements, Layer + 1,
        Geometry.ToPaintGeometry(FVector2f(94.0f * Scale, 62.0f * Scale), FSlateLayoutTransform(FVector2f(ItemPosition))),
        &Solid, ESlateDrawEffect::None, FLinearColor(0.015f, 0.015f, 0.015f, 0.35f));
    if (Item.Icon && Item.bOccupied)
    {
        const float Fit = 42.0f * Scale / FMath::Max(1, FMath::Max(Item.Icon->GetSizeX(), Item.Icon->GetSizeY()));
        const FVector2D Extent(Item.Icon->GetSizeX() * Fit, Item.Icon->GetSizeY() * Fit);
        Image(Item.Icon, ItemPosition + FVector2D(24.0f * Scale, 25.0f * Scale) - Extent * 0.5, Extent, White);
    }
    Text(ItemPosition + FVector2D(49.0f * Scale, 12.0f * Scale),
        FString::Printf(TEXT("×%d"), Item.bOccupied ? Item.Quantity : 0), 19,
        Item.bOccupied ? White : FLinearColor(1.0f, 1.0f, 1.0f, 0.35f));
    Text(ItemPosition + FVector2D(7.0f * Scale, 43.0f * Scale), TEXT("G"), 12, White);
    if (NameAge < 1.6f && Item.bOccupied)
    {
        Text(ItemPosition - FVector2D(0.0f, 28.0f * Scale), Item.Name.ToString(), 16,
            White.CopyWithNewOpacity(FMath::Clamp((1.6f - NameAge) / 0.3f, 0.0f, 1.0f)));
    }
    const FVector2D Center = Size * 0.5;
    if (Controller->ShouldShowCrosshair())
    {
        Image(Crosshair.Get(), Center - FVector2D(12.0f * Scale), FVector2D(24.0f * Scale), White);
    }

    if (!Controller->ShouldShowAimHud())
    {
        return Layer + 3;
    }

    FVector2D Actual;
    int32 ViewportX = 0;
    int32 ViewportY = 0;
    Controller->GetViewportSize(ViewportX, ViewportY);
    if (ViewportX > 0 && ViewportY > 0 && Controller->GetActualAimScreenPosition(Actual))
    {
        Actual *= FVector2D(Size.X / ViewportX, Size.Y / ViewportY);
        if (Actual.X >= 0 && Actual.Y >= 0 && Actual.X <= Size.X && Actual.Y <= Size.Y)
        {
            Arc(Actual, 8.0f, 0.0f, 360.0f, 1.0f, White);
        }
    }

    // 弹药细轨道独立于角落数字 保持准星右下短弧
    if (bHasAmmoDisplay)
    {
        BBBCombatAmmoRail::Paint(Elements, Geometry, Layer, Loaded, Capacity, bContinuous, AmmoColor);
    }
    return Layer + 3;
}
