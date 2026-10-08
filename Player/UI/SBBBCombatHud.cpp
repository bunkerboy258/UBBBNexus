#include "BBBWork/UBBBNexus/Player/UI/SBBBCombatHud.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "Engine/Texture2D.h"
#include "Rendering/DrawElementTypes.h"
#include "Brushes/SlateColorBrush.h"

void SBBBCombatHud::Construct(const FArguments &Arguments)
{
    View = Arguments._View;
    SetVisibility(EVisibility::HitTestInvisible);
    ForceVolatile(true);
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
    if (!Controller || !Controller->HasItemInventory())
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

    const float Health = Controller->GetHudHealthFraction();
    const double Half = 180.0 * Scale;
    const double Y = Size.Y - 38.0 * Scale;
    Line({FVector2D(Size.X * 0.5 - Half, Y), FVector2D(Size.X * 0.5 + Half, Y)},
        1.0f, FLinearColor(1.0f, 1.0f, 1.0f, 0.16f));
    if (Health > 0.0f)
    {
        Line({FVector2D(Size.X * 0.5 - Half * Health, Y), FVector2D(Size.X * 0.5 + Half * Health, Y)},
            2.0f, White);
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
    if (!Controller->ShouldShowAimHud())
    {
        return Layer + 2;
    }

    const FVector2D Center = Size * 0.5;
    static const FSlateColorBrush Dot(FLinearColor::White);
    FSlateDrawElement::MakeBox(Elements, Layer + 3,
        Geometry.ToPaintGeometry(FVector2f(3.0f * Scale), FSlateLayoutTransform(FVector2f(Center - FVector2D(1.5 * Scale)))),
        &Dot, ESlateDrawEffect::None, White);
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

    int32 Loaded = 0;
    int32 Capacity = 0;
    bool bContinuous = false;
    if (!Equipment || !Equipment->ReadAmmoDisplay(Loaded, Capacity, bContinuous) || Capacity <= 0)
    {
        return Layer + 3;
    }
    Loaded = FMath::Clamp(Loaded, 0, Capacity);
    const FLinearColor Track(1.0f, 1.0f, 1.0f, 0.20f);
    if (bContinuous)
    {
        Arc(Center, 31.0f, -66.0f, 66.0f, 2.0f, Track);
        Arc(Center, 31.0f, -66.0f, -66.0f + 132.0f * Loaded / Capacity, 2.0f, White);
        return Layer + 3;
    }
    const float Step = 132.0f / Capacity;
    for (int32 Index = 0; Index < Capacity; ++Index)
    {
        const float Begin = -66.0f + Step * Index;
        Arc(Center, 31.0f, Begin, Begin + Step * 0.70f, 3.0f, Index < Loaded ? White : Track);
    }
    return Layer + 3;
}
