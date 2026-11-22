#include "BBBWork/UBBBNexus/Player/UI/SBBBCombatHud.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBCombatAmmoRail.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "Brushes/SlateColorBrush.h"
#include "Engine/Texture2D.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/DrawElements.h"

void SBBBCombatHud::Construct(const FArguments &Arguments)
{
    View = Arguments._View;
    ItemIconMaterial = Arguments._ItemIconMaterial;
    SetVisibility(EVisibility::HitTestInvisible);
    ForceVolatile(true);
}

void SBBBCombatHud::Tick(const FGeometry &Geometry, double CurrentTime, float DeltaTime)
{
    SLeafWidget::Tick(Geometry, CurrentTime, DeltaTime);
    const ABBBPlayerController *Controller = View.IsValid() ? View->GetItemController() : nullptr;
    const double Now = FPlatformTime::Seconds();
    if (SampleStartedAt == 0.0)
    {
        SampleStartedAt = Now;
    }
    ++SampleFrames;
    const double SampleDuration = Now - SampleStartedAt;
    if (SampleDuration >= 0.5)
    {
        FrameRate = FMath::RoundToInt(SampleFrames / SampleDuration);
        Ping = Controller ? Controller->GetHudPing() : TOptional<int32>();
        SampleStartedAt = Now;
        SampleFrames = 0;
    }

    DamageAge += DeltaTime;
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
    if (bReset || Health > PreviousHealth)
    {
        DamageAge = 1.0f;
        DamageTrail = Fraction;
    }
    if (!bReset && Phase == 0 && Health < PreviousHealth)
    {
        const float PreviousFraction = PreviousHealth / FMath::Max(1.0f, Maximum);
        DamageTrail = FMath::Lerp(DamageTrail, PreviousFraction,
            FMath::Clamp((DamageAge - DeltaTime - 0.15f) / 0.45f, 0.0f, 1.0f));
        DamageAge = 0.0f;
    }
    bLowHealth = Phase == 0 && Fraction <= (bLowHealth && !bReset ? 0.35f : 0.30f);
    ObservedPawn = Controller->GetPawn();
    PreviousHealth = Health;
    PreviousMaximum = Maximum;
    PreviousPhase = Phase;
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
    const float Scale = FMath::Clamp(float(FMath::Min(Size.Y / 1080.0, Size.X / 900.0)), 0.25f, 2.0f);
    const FVector2D Center = Size * 0.5;
    const float ItemScale = Scale * 1.40f;
    const double Left = 40.0 * Scale;
    const double Right = Size.X - Left;
    const double Bottom = Size.Y - 58.0 * Scale;
    const FLinearColor White(1.0f, 1.0f, 1.0f, 0.96f);
    const FLinearColor Gray(0.72f, 0.72f, 0.72f, 0.78f);
    const FLinearColor Shadow(0.0f, 0.0f, 0.0f, 0.76f);
    const auto FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

    const auto Line = [&](const TArray<FVector2D> &Points, float Width, const FLinearColor &Color)
    {
        FSlateDrawElement::MakeLines(Elements, Layer + 1, Geometry.ToPaintGeometry(), Points,
            ESlateDrawEffect::None, Shadow.CopyWithNewOpacity(Color.A * 0.8f), true, (Width + 1.8f) * Scale);
        FSlateDrawElement::MakeLines(Elements, Layer + 2, Geometry.ToPaintGeometry(), Points,
            ESlateDrawEffect::None, Color, true, Width * Scale);
    };
    const auto Arc = [&](FVector2D Origin, float Radius, float From, float To, const FLinearColor &Color)
    {
        TArray<FVector2D> Points;
        const int32 Steps = FMath::Max(2, FMath::CeilToInt((To - From) / 15.0f));
        for (int32 Index = 0; Index <= Steps; ++Index)
        {
            const float Angle = FMath::DegreesToRadians(FMath::Lerp(From, To, float(Index) / Steps));
            Points.Add(Origin + FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius * Scale);
        }
        Line(Points, 1.35f, Color);
    };
    const auto Number = [&](const FString &Value, FVector2D Anchor, int32 FontSize, bool bRightAligned, const FLinearColor &Color)
    {
        auto Font = BBBItemUI::Font(FMath::Max(8, FMath::RoundToInt(FontSize * ItemScale)));
        Font.OutlineSettings.OutlineSize = FMath::Max(1, FMath::RoundToInt(Scale));
        Font.OutlineSettings.OutlineColor = Shadow;
        const FVector2D Extent = FontMeasure->Measure(Value, Font);
        const FVector2D Position = Anchor - FVector2D(bRightAligned ? Extent.X : 0.0, Extent.Y * 0.5);
        FSlateDrawElement::MakeText(Elements, Layer + 3,
            Geometry.ToPaintGeometry(Extent, FSlateLayoutTransform(Position)), Value, Font, ESlateDrawEffect::None, Color);
        return Extent.X;
    };
    TArray<UTexture2D *, TInlineAllocator<8>> UsedIcons;
    const auto Icon = [&](UTexture2D *Texture, FVector2D Anchor, FVector2D Limit, bool bRightAligned, const FLinearColor &Color)
    {
        if (!Texture || !ItemIconMaterial.IsValid() || Texture->GetSizeX() <= 0 || Texture->GetSizeY() <= 0)
        {
            return 0.0;
        }
        UsedIcons.AddUnique(Texture);
        auto &Material = IconMaterials.FindOrAdd(Texture);
        if (!Material.IsValid())
        {
            Material.Reset(UMaterialInstanceDynamic::Create(ItemIconMaterial.Get(), View.Get()));
            Material->SetTextureParameterValue(TEXT("ImageTexture"), Texture);
        }
        const FVector2D Source(Texture->GetSizeX(), Texture->GetSizeY());
        const FVector2D Extent = Source * (ItemScale * FMath::Min(Limit.X / Source.X, Limit.Y / Source.Y));
        const FVector2D Position = Anchor - FVector2D(bRightAligned ? Extent.X : 0.0, Extent.Y * 0.5);
        FSlateBrush Brush;
        Brush.SetResourceObject(Material.Get());
        Brush.ImageSize = Source;
        FSlateDrawElement::MakeBox(Elements, Layer + 1, Geometry.ToPaintGeometry(Extent,
            FSlateLayoutTransform(Position + FVector2D(1.0, 1.5) * Scale)), &Brush, ESlateDrawEffect::None,
            Shadow.CopyWithNewOpacity(Color.A * 0.8f));
        FSlateDrawElement::MakeBox(Elements, Layer + 2, Geometry.ToPaintGeometry(Extent,
            FSlateLayoutTransform(Position)), &Brush, ESlateDrawEffect::None, Color);
        return Extent.X;
    };

    static const FSlateColorBrush Solid(FLinearColor::White);
    const double HalfBar = FMath::Min(Size.X * 0.19, 315.0 * Scale);
    const auto Meter = [&](double Y, float Height, float Fraction, const FLinearColor &Color, int32 Offset)
    {
        if (Fraction <= 0.0f)
        {
            return;
        }
        const double Half = HalfBar * Fraction;
        FSlateDrawElement::MakeBox(Elements, Layer + Offset,
            Geometry.ToPaintGeometry(FVector2D(2.0 * Half, Height * Scale),
                FSlateLayoutTransform(FVector2D(Center.X - Half, Y))), &Solid, ESlateDrawEffect::None, Color);
    };
    const double HealthY = Size.Y - 78.0 * Scale;
    const double StaminaY = HealthY - 22.0 * Scale;
    const float Health = Controller->GetHudHealthFraction();
    const float Trail = FMath::Lerp(DamageTrail, Health, FMath::Clamp((DamageAge - 0.15f) / 0.45f, 0.0f, 1.0f));
    const float LowPulse = bLowHealth ? 0.68f + 0.28f * FMath::Abs(FMath::Sin(float(FPlatformTime::Seconds() * 4.0))) : White.A;
    Meter(StaminaY - Scale, 5.0f, 1.0f, Shadow, 0);
    Meter(StaminaY, 3.0f, Controller->GetHudStaminaFraction(), Gray, 2);
    Meter(HealthY - Scale, 8.5f, 1.0f, Shadow, 0);
    Meter(HealthY, 6.5f, Trail, FLinearColor(0.38f, 0.38f, 0.38f, 0.9f), 1);
    Meter(HealthY, 6.5f, Health, White.CopyWithNewOpacity(LowPulse), 2);

    int32 ConsumableStart = 0;
    int32 ConsumableCount = 0;
    Controller->GetConsumableQuickRange(ConsumableStart, ConsumableCount);
    const int32 Selected = Controller->GetSelectedThrowableSlot();
    const bool bHasSelected = Selected >= 0 && Selected < ConsumableCount;
    if (bHasSelected)
    {
        const auto Item = Controller->GetItemDisplayData(ConsumableStart + Selected);
        const double Y = Bottom - 14.0 * ItemScale;
        Icon(Item.Icon, FVector2D(Left, Y), FVector2D(52.0, 42.0), false, White);
        Number(FString::FromInt(Item.Quantity), FVector2D(Left + 57.0 * ItemScale, Y), 28, false, White);
    }
    double ConsumableY = Bottom - (bHasSelected ? 55.0 : 14.0) * ItemScale;
    for (int32 Index = 0; Index < ConsumableCount; ++Index)
    {
        const auto Item = Controller->GetItemDisplayData(ConsumableStart + Index);
        if (!Item.bOccupied || Index == Selected)
        {
            continue;
        }
        Icon(Item.Icon, FVector2D(Left, ConsumableY), FVector2D(30.0, 24.0), false, Gray);
        Number(FString::Printf(TEXT("%02d"), Item.Quantity), FVector2D(Left + 36.0 * ItemScale, ConsumableY), 16, false, Gray);
        ConsumableY -= 28.0 * ItemScale;
    }

    int32 Loaded = 0;
    int32 Capacity = 0;
    bool bContinuous = false;
    bool bReloading = false;
    const bool bHasAmmo = Controller->ReadHudAmmo(Loaded, Capacity, bContinuous, bReloading);
    const FLinearColor AmmoColor = Loaded > 0 ? White : Gray;
    const double AmmoY = Bottom - 3.0 * Scale;
    if (bHasAmmo)
    {
        const double CapacityWidth = Number(FString::Printf(TEXT("/%d"), Capacity), FVector2D(Right, AmmoY + 2.0 * Scale), 17, true, Gray);
        const double LoadedWidth = Number(FString::FromInt(Loaded), FVector2D(Right - CapacityWidth - 3.0 * Scale, AmmoY), 29, true, AmmoColor);
        if (bReloading)
        {
            const double RingX = Right - CapacityWidth - LoadedWidth - 17.0 * Scale;
            const float Angle = float(FMath::Fmod(FPlatformTime::Seconds() * 360.0, 360.0));
            Arc(FVector2D(RingX, AmmoY), 6.0f, 0.0f, 360.0f, Gray.CopyWithNewOpacity(0.35f));
            Arc(FVector2D(RingX, AmmoY), 6.0f, Angle, Angle + 125.0f, White);
        }
    }
    const double WeaponY = Bottom - (bHasAmmo ? 40.0 : 14.0) * ItemScale;
    const double WeaponWidth = Icon(Controller->GetHudWeaponIcon(), FVector2D(Right, WeaponY), FVector2D(124.0, 38.0), true, White);
    double SlotY = WeaponY - (WeaponWidth > 0.0 ? 39.0 : 0.0) * ItemScale;
    for (int32 Index = 0; Index < Controller->GetQuickAccessSlotCount(); ++Index)
    {
        const auto Item = Controller->GetItemDisplayData(Index);
        if (!Item.bOccupied || Item.bActive)
        {
            continue;
        }
        Icon(Item.Icon, FVector2D(Right, SlotY), FVector2D(58.0, 23.0), true, Gray);
        Number(FString::Printf(TEXT("%02d"), Item.QuickIndex + 1), FVector2D(Right - 65.0 * ItemScale, SlotY), 16, true, Gray);
        SlotY -= 27.0 * ItemScale;
    }
    for (auto It = IconMaterials.CreateIterator(); It; ++It)
    {
        if (!UsedIcons.Contains(It.Key().Get()))
        {
            It.RemoveCurrent();
        }
    }

    const double TelemetryY = 37.0 * Scale;
    double TelemetryX = Right;
    if (Ping.IsSet())
    {
        TelemetryX -= Number(FString::FromInt(Ping.GetValue()), FVector2D(TelemetryX, TelemetryY), 16, true, Gray);
        const double SignalX = TelemetryX - 8.0 * Scale;
        for (int32 Bar = 0; Bar < 3; ++Bar)
        {
            const double X = SignalX - (2 - Bar) * 3.0 * Scale;
            Line({FVector2D(X, TelemetryY + 4.0 * Scale), FVector2D(X, TelemetryY + (1.0 - 3.0 * Bar) * Scale)}, 1.5f, Gray);
        }
        TelemetryX -= 28.0 * Scale;
    }
    if (FrameRate > 0)
    {
        TelemetryX -= Number(FString::FromInt(FrameRate), FVector2D(TelemetryX, TelemetryY), 16, true, Gray);
        const double X = TelemetryX - 14.0 * Scale;
        Line({FVector2D(X, TelemetryY + 5.0 * Scale), FVector2D(X, TelemetryY - 4.0 * Scale),
            FVector2D(X + 10.0 * Scale, TelemetryY - 4.0 * Scale), FVector2D(X + 10.0 * Scale, TelemetryY + 5.0 * Scale),
            FVector2D(X, TelemetryY + 5.0 * Scale)}, 1.0f, Gray);
    }

    if (Controller->ShouldShowCrosshair())
    {
        for (const FVector2D Axis : {FVector2D(0.0, 1.0), FVector2D(0.0, -1.0), FVector2D(1.0, 0.0), FVector2D(-1.0, 0.0)})
        {
            Line({Center + Axis * (5.0 * Scale), Center + Axis * (11.0 * Scale)}, 1.0f, White);
        }
        Line({Center - FVector2D(0.65 * Scale, 0.0), Center + FVector2D(0.65 * Scale, 0.0)}, 1.2f, White);
    }
    if (Controller->ShouldShowAimHud())
    {
        FVector2D Actual;
        int32 ViewportX = 0;
        int32 ViewportY = 0;
        Controller->GetViewportSize(ViewportX, ViewportY);
        if (ViewportX > 0 && ViewportY > 0 && Controller->GetActualAimScreenPosition(Actual))
        {
            Actual *= FVector2D(Size.X / ViewportX, Size.Y / ViewportY);
            if (Actual.X >= 0.0 && Actual.Y >= 0.0 && Actual.X <= Size.X && Actual.Y <= Size.Y)
            {
                Arc(Actual, 8.0f, 0.0f, 360.0f, White);
            }
        }
        if (bHasAmmo)
        {
            BBBCombatAmmoRail::Paint(Elements, Geometry, Layer, Loaded, Capacity, bContinuous, AmmoColor);
        }
    }
    return Layer + 3;
}
