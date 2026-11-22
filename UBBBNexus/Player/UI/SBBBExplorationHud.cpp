#include "BBBWork/UBBBNexus/Player/UI/SBBBExplorationHud.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStyle.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "Brushes/SlateColorBrush.h"
#include "Rendering/DrawElements.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"
#include "Test/V01/Gameplay/BBBV01TutorialPresentation.h"

void SBBBExplorationHud::Construct(const FArguments &Arguments)
{
    View = Arguments._View;
    SetVisibility(EVisibility::HitTestInvisible);
    ForceVolatile(true);
}

FVector2D SBBBExplorationHud::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return FVector2D::ZeroVector;
}

int32 SBBBExplorationHud::OnPaint(const FPaintArgs &Args, const FGeometry &Geometry,
    const FSlateRect &CullingRect, FSlateWindowElementList &Elements, int32 Layer,
    const FWidgetStyle &Style, bool bParentEnabled) const
{
    const auto *Owner = View.Get();
    const auto *Controller = Owner ? Owner->GetItemController() : nullptr;
    if (!Controller)
    {
        return Layer;
    }
    const FVector2D Size = Geometry.GetLocalSize();
    const float Scale = FMath::Clamp(float(Size.Y / 1080.0), 0.55f, 1.6f);
    static const FSlateColorBrush Solid(FLinearColor::White);
    const auto Box = [&](FVector2D Position, FVector2D Extent, FLinearColor Color, int32 Offset)
    {
        FSlateDrawElement::MakeBox(Elements, Layer + Offset,
            Geometry.ToPaintGeometry(Extent, FSlateLayoutTransform(Position)), &Solid,
            ESlateDrawEffect::None, Color);
    };
    const auto Text = [&](const FString &Value, FVector2D Position, const FSlateFontInfo &Font,
        FLinearColor Color, int32 Offset)
    {
        FSlateDrawElement::MakeText(Elements, Layer + Offset,
            Geometry.ToPaintGeometry(Size, FSlateLayoutTransform(Position)), Value, Font,
            ESlateDrawEffect::None, Color);
    };
    const double CompletedAt = Controller->GetExplorationCompletionTime();
    const double Age = FPlatformTime::Seconds() - CompletedAt;
    if (CompletedAt >= 0.0 && Age >= 0.0 && Age < 3.0)
    {
        const float Alpha = FMath::Min(FMath::Clamp(float(Age / 0.12), 0.0f, 1.0f),
            FMath::Clamp(float((3.0 - Age) / 0.28), 0.0f, 1.0f));
        const auto Font = BBBItemUI::Font(FMath::RoundToInt(148.0f * Scale));
        const FString Title(TEXT("Evac"));
        const FVector2D Extent = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(Title, Font);
        const FVector2D Center = Size * 0.5f;
        const FVector2D Position = Center - Extent * 0.5f;
        Box(FVector2D(0.0f, Center.Y - 110.0f * Scale), FVector2D(Size.X, 220.0f * Scale),
            FLinearColor(0.005f, 0.006f, 0.008f, 0.78f * Alpha), 3);
        Box(FVector2D(Center.X - 110.0f * Scale, Center.Y + 92.0f * Scale),
            FVector2D(220.0f, 3.0f) * Scale, BBBItemUI::Accent.CopyWithNewOpacity(Alpha), 4);
        Text(Title, Position + FVector2D(3.0f, 4.0f) * Scale, Font, FLinearColor(0, 0, 0, Alpha), 4);
        Text(Title, Position, Font, FLinearColor(1, 1, 1, Alpha), 5);
    }
    return FBBBV01TutorialPresentation::Paint(*Controller, Geometry, Elements, Layer + 5);
}
