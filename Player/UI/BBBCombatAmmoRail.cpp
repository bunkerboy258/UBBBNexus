#include "BBBWork/UBBBNexus/Player/UI/BBBCombatAmmoRail.h"
#include "Layout/Geometry.h"
#include "Rendering/DrawElements.h"

void BBBCombatAmmoRail::Paint(FSlateWindowElementList &Elements, const FGeometry &Geometry, const int32 Layer,
    int32 Loaded, const int32 Capacity, const bool bContinuous, const FLinearColor &Color)
{
    if (Capacity <= 0)
    {
        return;
    }
    Loaded = FMath::Clamp(Loaded, 0, Capacity);
    const FVector2D Size = Geometry.GetLocalSize();
    const FVector2D Center = Size * 0.5;
    const float Scale = FMath::Clamp(static_cast<float>(Size.Y / 1080.0), 0.55f, 1.6f);
    // 已确认的弹药轨道构图 半径五十 起始二十度 跨度六十度 不随角落弹药数字布局移动
    constexpr float Radius = 50.0f;
    constexpr float From = 20.0f;
    constexpr float Sweep = 60.0f;
    const FLinearColor Track(1.0f, 1.0f, 1.0f, 0.20f);
    const auto Arc = [&](const float Begin, const float End, const float Width, const FLinearColor &Tint)
    {
        if (End <= Begin)
        {
            return;
        }
        TArray<FVector2D> Points;
        const int32 Steps = FMath::Max(2, FMath::CeilToInt((End - Begin) / 4.0f));
        for (int32 Index = 0; Index <= Steps; ++Index)
        {
            const float Angle = FMath::DegreesToRadians(FMath::Lerp(Begin, End, static_cast<float>(Index) / Steps));
            Points.Add(Center + FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * Radius * Scale);
        }
        FSlateDrawElement::MakeLines(Elements, Layer + 1, Geometry.ToPaintGeometry(), Points,
            ESlateDrawEffect::None, FLinearColor(0.0f, 0.0f, 0.0f, Tint.A * 0.55f), true, (Width + 2.0f) * Scale);
        FSlateDrawElement::MakeLines(Elements, Layer + 2, Geometry.ToPaintGeometry(), Points,
            ESlateDrawEffect::None, Tint, true, Width * Scale);
    };
    if (bContinuous)
    {
        Arc(From, From + Sweep, 2.0f, Track);
        Arc(From, From + Sweep * Loaded / Capacity, 2.0f, Color);
        return;
    }
    const float Step = Sweep / Capacity;
    for (int32 Index = 0; Index < Capacity; ++Index)
    {
        const float Begin = From + Step * Index;
        Arc(Begin, Begin + Step * 0.70f, 3.0f, Index < Loaded ? Color : Track);
    }
}
