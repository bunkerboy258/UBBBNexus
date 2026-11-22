#include "BBBWork/UBBBNexus/Player/UI/BBBCombatHudIndicator.h"

#include "BBBWork/UBBBNexus/Player/UI/BBBCombatAmmoRail.h"
#include "Rendering/DrawElements.h"
#include "Widgets/SLeafWidget.h"

/** 仅根据控件类型和已提供的数据绘制图形 */
class SBBBCombatHudIndicator final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBBBCombatHudIndicator) {}
    SLATE_END_ARGS()

    void Construct(const FArguments &Arguments)
    {
        SetVisibility(EVisibility::HitTestInvisible);
    }

    virtual FVector2D ComputeDesiredSize(float) const override
    {
        return FVector2D(24.0, 24.0);
    }

    virtual int32 OnPaint(const FPaintArgs &, const FGeometry &Geometry, const FSlateRect &,
        FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &, bool) const override
    {
        if (Glyph == EBBBCombatHudGlyph::AmmoRail)
        {
            const FLinearColor AmmoTint = Loaded > 0 ? Tint : FLinearColor(0.72f, 0.72f, 0.72f, 0.78f);
            BBBCombatAmmoRail::Paint(Elements, Geometry, Layer, Loaded, Capacity, bContinuous, AmmoTint);
            return Layer + 2;
        }

        const FVector2D Size = Geometry.GetLocalSize();
        const FVector2D Center = Size * 0.5;
        const float Scale = FMath::Min(Size.X, Size.Y) / 24.0;
        const auto Line = [&](const TArray<FVector2D> &Points, float Width, const FLinearColor &Color)
        {
            const FPaintGeometry PaintGeometry = Geometry.ToPaintGeometry(
                FVector2D(24.0, 24.0), FSlateLayoutTransform(Scale, Center - FVector2D(12.0) * Scale));
            FSlateDrawElement::MakeLines(Elements, Layer + 1, PaintGeometry, Points, ESlateDrawEffect::None,
                FLinearColor(0.0f, 0.0f, 0.0f, Color.A * 0.65f), true, Width + 1.8f);
            FSlateDrawElement::MakeLines(Elements, Layer + 2, PaintGeometry, Points,
                ESlateDrawEffect::None, Color, true, Width);
        };
        const auto Arc = [&](float Radius, float From, float Sweep, const FLinearColor &Color)
        {
            TArray<FVector2D> Points;
            const int32 Steps = FMath::CeilToInt(Sweep / 15.0f);
            for (int32 Index = 0; Index <= Steps; ++Index)
            {
                const float Angle = FMath::DegreesToRadians(From + Sweep * Index / Steps);
                Points.Add(FVector2D(12.0) + FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius);
            }
            Line(Points, 1.35f, Color);
        };

        const FVector2D Origin(12.0);
        switch (Glyph)
        {
        case EBBBCombatHudGlyph::Crosshair:
            for (const FVector2D Axis : {FVector2D(1.0, 0.0), FVector2D(-1.0, 0.0), FVector2D(0.0, 1.0), FVector2D(0.0, -1.0)})
            {
                Line({Origin + Axis * 5.0, Origin + Axis * 11.0}, 1.0f, Tint);
            }
            Line({Origin - FVector2D(0.65, 0.0), Origin + FVector2D(0.65, 0.0)}, 1.2f, Tint);
            break;
        case EBBBCombatHudGlyph::AimPoint:
            Arc(8.0f, 0.0f, 360.0f, Tint);
            break;
        case EBBBCombatHudGlyph::Reload:
            Arc(8.0f, 0.0f, 360.0f, Tint.CopyWithNewOpacity(Tint.A * 0.35f));
            Arc(8.0f, ReloadAngle, 125.0f, Tint);
            break;
        case EBBBCombatHudGlyph::FrameRate:
            Line({{3.0, 3.0}, {21.0, 3.0}, {21.0, 16.0}, {3.0, 16.0}, {3.0, 3.0}}, 1.25f, Tint);
            Line({{7.0, 21.0}, {17.0, 21.0}}, 1.25f, Tint);
            break;
        case EBBBCombatHudGlyph::Ping:
            for (int32 Bar = 0; Bar < 3; ++Bar)
            {
                const double X = 5.0 + Bar * 7.0;
                Line({{X, 20.0}, {X, 15.0 - Bar * 6.0}}, 2.0f, Tint);
            }
            break;
        default:
            break;
        }
        return Layer + 2;
    }

    EBBBCombatHudGlyph Glyph = EBBBCombatHudGlyph::Crosshair;
    FLinearColor Tint = FLinearColor::White;
    int32 Loaded = 9;
    int32 Capacity = 15;
    bool bContinuous = true;
    float ReloadAngle = 0.0f;
};

TSharedRef<SWidget> UBBBCombatHudIndicator::RebuildWidget()
{
    Indicator = SNew(SBBBCombatHudIndicator);
    return Indicator.ToSharedRef();
}

void UBBBCombatHudIndicator::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    if (Indicator)
    {
        Indicator->Glyph = Glyph;
        Indicator->Tint = Tint;
        Indicator->Invalidate(EInvalidateWidgetReason::Paint);
    }
}

void UBBBCombatHudIndicator::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    Indicator.Reset();
}

void UBBBCombatHudIndicator::SetAmmo(int32 Loaded, int32 Capacity, bool bContinuous)
{
    if (Indicator && (Indicator->Loaded != Loaded || Indicator->Capacity != Capacity || Indicator->bContinuous != bContinuous))
    {
        Indicator->Loaded = Loaded;
        Indicator->Capacity = Capacity;
        Indicator->bContinuous = bContinuous;
        Indicator->Invalidate(EInvalidateWidgetReason::Paint);
    }
}

void UBBBCombatHudIndicator::SetReloadAngle(float Angle)
{
    if (Indicator && Indicator->ReloadAngle != Angle)
    {
        Indicator->ReloadAngle = Angle;
        Indicator->Invalidate(EInvalidateWidgetReason::Paint);
    }
}
