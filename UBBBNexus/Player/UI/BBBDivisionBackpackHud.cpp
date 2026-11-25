#include "BBBWork/UBBBNexus/Player/UI/BBBDivisionBackpackHud.h"

#include "Brushes/SlateColorBrush.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Fonts/CompositeFont.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Rendering/DrawElements.h"
#include "UObject/ConstructorHelpers.h"
#include "Widgets/SLeafWidget.h"

namespace
{
/** 根据字体资产构造无回退复合字体 */
TSharedPtr<const FCompositeFont> BuildNumericCompositeFont(UFontFace *Face)
{
    if (!Face)
    {
        return nullptr;
    }

    TSharedPtr<FCompositeFont> Composite = MakeShared<FCompositeFont>();
    Composite->DefaultTypeface.Fonts.AddDefaulted_GetRef().Font = FFontData(Face);
    return Composite;
}

/** 计算等比居中适应盒后的绘制区域 支持对正方形枪械剪影去除上下留白缩放 */
void FitAspectBox(const FVector2D &BoxOrigin, const FVector2D &BoxSize, const FVector2D &ImageSize,
    bool bSquareWeaponTrim, FVector2D &OutOrigin, FVector2D &OutSize)
{
    if (BoxSize.X <= 0.0 || BoxSize.Y <= 0.0 || ImageSize.X <= 0.0 || ImageSize.Y <= 0.0)
    {
        OutOrigin = BoxOrigin;
        OutSize = FVector2D::ZeroVector;
        return;
    }

    const double Aspect = ImageSize.X / ImageSize.Y;
    if (bSquareWeaponTrim && FMath::Abs(Aspect - 1.0) < 0.18)
    {
        // 项目枪械剪影为正方形且实心轮廓位于中部约 42% 高度区域 按有效实心高度缩放可饱满呈现枪形
        constexpr double ContentHeightFraction = 0.42;
        const double TargetEdge = FMath::Min(BoxSize.X, BoxSize.Y / ContentHeightFraction);
        OutSize = FVector2D(TargetEdge, TargetEdge);
        OutOrigin = BoxOrigin + (BoxSize - OutSize) * 0.5;
        return;
    }

    const double Ratio = FMath::Min(BoxSize.X / ImageSize.X, BoxSize.Y / ImageSize.Y);
    OutSize = ImageSize * Ratio;
    OutOrigin = BoxOrigin + (BoxSize - OutSize) * 0.5;
}
}

/** 全境封锁 2 风格开放悬空极简白色微自发光背包侧边战术集群绘制器 */
class SBBBDivisionBackpackHud final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SBBBDivisionBackpackHud) {}
    SLATE_END_ARGS()

    void Construct(const FArguments &Arguments)
    {
        SetVisibility(EVisibility::HitTestInvisible);
    }

    virtual FVector2D ComputeDesiredSize(float) const override
    {
        return FVector2D::ZeroVector;
    }

    void ApplyFonts(UFontFace *BoldFace, UFontFace *LightFace)
    {
        if (CachedBoldFace.Get() != BoldFace)
        {
            CachedBoldFace = BoldFace;
            BoldComposite = BuildNumericCompositeFont(BoldFace);
        }

        if (CachedLightFace.Get() != LightFace)
        {
            CachedLightFace = LightFace;
            LightComposite = BuildNumericCompositeFont(LightFace);
        }
    }

    void RefreshCachedNumbers()
    {
        if (CachedLoaded != Snapshot.LoadedAmmo)
        {
            CachedLoaded = Snapshot.LoadedAmmo;
            LoadedText = FString::FromInt(CachedLoaded);
        }

        if (CachedCapacity != Snapshot.CapacityAmmo)
        {
            CachedCapacity = Snapshot.CapacityAmmo;
            CapacityText = FString::Printf(TEXT("/%d"), CachedCapacity);
        }

        if (CachedSelectedThrowableCount != Snapshot.SelectedThrowable.Number)
        {
            CachedSelectedThrowableCount = Snapshot.SelectedThrowable.Number;
            SelectedThrowableText = CachedSelectedThrowableCount >= 0
                ? FString::FromInt(CachedSelectedThrowableCount)
                : FString();
        }

        for (int32 Index = 0; Index < FBBBDivisionHudSnapshot::MaxSpareThrowables; ++Index)
        {
            const int32 Value = Snapshot.SpareThrowables[Index].Number;
            if (CachedSpareThrowableCounts[Index] != Value)
            {
                CachedSpareThrowableCounts[Index] = Value;
                SpareThrowableTexts[Index] = Value >= 0 ? FString::FromInt(Value) : FString();
            }
        }
    }

    virtual int32 OnPaint(const FPaintArgs &Args, const FGeometry &Geometry, const FSlateRect &CullingRect,
        FSlateWindowElementList &Elements, int32 Layer, const FWidgetStyle &Style, bool bParentEnabled) const override
    {
        const FVector2D LocalSize = Geometry.GetLocalSize();
        if (LocalSize.X <= 1.0 || LocalSize.Y <= 1.0 || !BoldComposite.IsValid() || !LightComposite.IsValid())
        {
            return Layer;
        }

        const float ViewportScale = FMath::Clamp(static_cast<float>(LocalSize.Y / 1080.0), 0.65f, 1.55f);
        const float Scale = ViewportScale * Snapshot.DistanceScale;

        const bool bShowWeaponBand = Snapshot.bHasAmmo || Snapshot.bHasActiveWeapon;
        const bool bHasThrowables = Snapshot.SelectedThrowable.bOccupied
            || Snapshot.SpareThrowables[0].bOccupied
            || Snapshot.SpareThrowables[1].bOccupied;

        // 常驻生命与装备栏固定锚定 武器弹药区固定预留于其上方 切枪与空手时下方常驻栏零位移
        const float ClusterWidth = 268.0f * Scale;
        const float WeaponBandHeight = 40.0f * Scale;
        const float WeaponGap = 7.0f * Scale;
        const float TopReservedHeight = WeaponBandHeight + WeaponGap;
        const float HealthBarHeight = 9.5f * Scale;
        const float StaminaGap = 4.5f * Scale;
        const float StaminaHeight = 2.8f * Scale;
        const float VitalsHeight = HealthBarHeight + StaminaGap + StaminaHeight;
        const float LoadoutGap = 11.0f * Scale;
        const float LoadoutHeight = 28.0f * Scale;
        const float PersistentHeight = VitalsHeight + LoadoutGap + LoadoutHeight;
        const float FullClusterHeight = TopReservedHeight + PersistentHeight;

        // 屏幕空间锚点锁定在常驻体征栏基准线 瞄准与空手均不减弱显示也不挤压常驻栏
        FVector2D Anchor = Snapshot.bHasAnchor
            ? Snapshot.AnchorPosition
            : FVector2D(LocalSize.X * 0.54, LocalSize.Y * 0.62);
        Anchor += AnchorPixelOffset * Scale;

        const double SafeEdgeX = 28.0 * ViewportScale;
        const double SafeEdgeTop = 28.0 * ViewportScale + FMath::Max(0.0f, -PerspectiveSlopeY) * ClusterWidth;
        const double SafeEdgeBottom = 28.0 * ViewportScale + FMath::Max(0.0f, PerspectiveSlopeY) * ClusterWidth;
        const double ReticleAvoidLeft = LocalSize.X * 0.5 + 52.0 * ViewportScale;
        const double ReticleAvoidTop = LocalSize.Y * 0.5 - 56.0 * ViewportScale;
        const double ReticleAvoidBottom = LocalSize.Y * 0.5 + 56.0 * ViewportScale;

        FVector2D Origin(Anchor.X, Anchor.Y - TopReservedHeight);
        if (Origin.Y < ReticleAvoidBottom && (Origin.Y + FullClusterHeight) > ReticleAvoidTop && Origin.X < ReticleAvoidLeft)
        {
            Origin.X = FMath::Max(Origin.X, ReticleAvoidLeft);
        }

        Origin.X = FMath::Clamp(Origin.X, SafeEdgeX, FMath::Max(SafeEdgeX, LocalSize.X - ClusterWidth - SafeEdgeX));
        Origin.Y = FMath::Clamp(Origin.Y, SafeEdgeTop, FMath::Max(SafeEdgeTop, LocalSize.Y - FullClusterHeight - SafeEdgeBottom));

        // 鲜明的全境封锁 2 立体透视斜切 枢轴锁定在常驻体征栏左缘 确保持枪/空手变换中心完全一致
        const float PivotY = TopReservedHeight / FMath::Max(1.0f, FullClusterHeight);
        const FMatrix2x2 PerspectiveMatrix(1.0f, PerspectiveSlopeY, PerspectiveShearX, 1.0f);
        const FSlateRenderTransform PerspectiveTransform(PerspectiveMatrix);
        const FGeometry ClusterGeometry = Geometry.MakeChild(
            FVector2f(ClusterWidth, FullClusterHeight),
            FSlateLayoutTransform(FVector2f(Origin)),
            PerspectiveTransform,
            FVector2f(0.0f, PivotY));

        static const FSlateColorBrush SolidBrush(FLinearColor::White);
        const float ShadowAlphaScale = FMath::Clamp(ContrastShadowStrength, 0.0f, 1.0f);
        const float GlowScale = FMath::Clamp(EmissiveGlowStrength, 0.0f, 2.0f);
        const FLinearColor GlowColor(0.93f, 0.97f, 1.0f, 1.0f);

        const auto AlphaTint = [](const FLinearColor &Base, float ExtraAlpha = 1.0f)
        {
            return Base.CopyWithNewOpacity(Base.A * ExtraAlpha);
        };

        const auto DrawRect = [&](const FVector2D &Pos, const FVector2D &Extent, const FLinearColor &Color, int32 LayerOffset)
        {
            if (Extent.X <= 0.0 || Extent.Y <= 0.0 || Color.A <= 0.002f)
            {
                return;
            }

            FSlateDrawElement::MakeBox(
                Elements,
                Layer + LayerOffset,
                ClusterGeometry.ToPaintGeometry(Extent, FSlateLayoutTransform(FVector2f(Pos))),
                &SolidBrush,
                ESlateDrawEffect::NoPixelSnapping,
                Color);
        };

        // 轻量纯白微自发光光晕 三层指数柔扩 零离屏模糊开销
        const auto DrawRectGlow = [&](const FVector2D &Pos, const FVector2D &Extent, float Intensity, int32 LayerOffset)
        {
            if (GlowScale <= 0.01f || Intensity <= 0.01f || Extent.X <= 0.0 || Extent.Y <= 0.0)
            {
                return;
            }

            const float OuterExpand = 6.5f * Scale;
            const float MidExpand = 3.2f * Scale;
            const float InnerExpand = 1.3f * Scale;
            DrawRect(Pos - FVector2D(OuterExpand, OuterExpand),
                Extent + FVector2D(OuterExpand * 2.0f, OuterExpand * 2.0f),
                GlowColor.CopyWithNewOpacity(FMath::Clamp(0.055f * Intensity * GlowScale, 0.0f, 0.18f)),
                LayerOffset);
            DrawRect(Pos - FVector2D(MidExpand, MidExpand),
                Extent + FVector2D(MidExpand * 2.0f, MidExpand * 2.0f),
                GlowColor.CopyWithNewOpacity(FMath::Clamp(0.12f * Intensity * GlowScale, 0.0f, 0.30f)),
                LayerOffset);
            DrawRect(Pos - FVector2D(InnerExpand, InnerExpand),
                Extent + FVector2D(InnerExpand * 2.0f, InnerExpand * 2.0f),
                GlowColor.CopyWithNewOpacity(FMath::Clamp(0.22f * Intensity * GlowScale, 0.0f, 0.44f)),
                LayerOffset);
        };

        const auto DrawLine = [&](const TArray<FVector2D> &Points, float Thickness, const FLinearColor &Color,
            int32 LayerOffset, bool bShadow = true, float GlowIntensity = 0.0f)
        {
            if (Points.Num() < 2 || Color.A <= 0.002f)
            {
                return;
            }

            if (bShadow && ShadowAlphaScale > 0.01f)
            {
                FSlateDrawElement::MakeLines(
                    Elements,
                    Layer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(),
                    Points,
                    ESlateDrawEffect::NoPixelSnapping,
                    FLinearColor(0.0f, 0.0f, 0.0f, Color.A * 0.65f * ShadowAlphaScale),
                    true,
                    Thickness + 1.8f * Scale);
            }

            if (GlowScale > 0.01f && GlowIntensity > 0.01f)
            {
                FSlateDrawElement::MakeLines(
                    Elements,
                    Layer + LayerOffset + 1,
                    ClusterGeometry.ToPaintGeometry(),
                    Points,
                    ESlateDrawEffect::NoPixelSnapping,
                    GlowColor.CopyWithNewOpacity(FMath::Clamp(0.10f * GlowIntensity * GlowScale * Color.A, 0.0f, 0.28f)),
                    true,
                    Thickness + 5.5f * Scale);
                FSlateDrawElement::MakeLines(
                    Elements,
                    Layer + LayerOffset + 1,
                    ClusterGeometry.ToPaintGeometry(),
                    Points,
                    ESlateDrawEffect::NoPixelSnapping,
                    GlowColor.CopyWithNewOpacity(FMath::Clamp(0.18f * GlowIntensity * GlowScale * Color.A, 0.0f, 0.40f)),
                    true,
                    Thickness + 2.6f * Scale);
            }

            FSlateDrawElement::MakeLines(
                Elements,
                Layer + LayerOffset + 2,
                ClusterGeometry.ToPaintGeometry(),
                Points,
                ESlateDrawEffect::NoPixelSnapping,
                Color,
                true,
                Thickness);
        };

        // 暗影保底 + 外扩微自发光光晕 + 纯白主体 绘制立体发光数字
        const auto DrawTextLuminous = [&](const FString &Text, const FVector2D &Pos, const FSlateFontInfo &Font,
            const FLinearColor &Color, float GlowIntensity, int32 LayerOffset)
        {
            if (Text.IsEmpty() || Color.A <= 0.002f)
            {
                return;
            }

            if (ShadowAlphaScale > 0.01f)
            {
                const float SAlpha = FMath::Max(Color.A, 0.78f) * ShadowAlphaScale;
                FSlateDrawElement::MakeText(
                    Elements,
                    Layer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, FullClusterHeight),
                        FSlateLayoutTransform(FVector2f(Pos + FVector2D(0.0, 1.5 * Scale)))),
                    Text,
                    Font,
                    ESlateDrawEffect::None,
                    FLinearColor(0.0f, 0.0f, 0.0f, SAlpha * 0.78f));

                FSlateDrawElement::MakeText(
                    Elements,
                    Layer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, FullClusterHeight),
                        FSlateLayoutTransform(FVector2f(Pos + FVector2D(1.3 * Scale, 1.0 * Scale)))),
                    Text,
                    Font,
                    ESlateDrawEffect::None,
                    FLinearColor(0.0f, 0.0f, 0.0f, SAlpha * 0.55f));
            }

            if (GlowScale > 0.01f && GlowIntensity > 0.01f)
            {
                const float InnerOff = 1.1f * Scale;
                const FLinearColor InnerHalo = GlowColor.CopyWithNewOpacity(
                    FMath::Clamp(0.18f * GlowIntensity * GlowScale * Color.A, 0.0f, 0.42f));
                const FVector2D InnerOffsets[4] = {
                    FVector2D(-InnerOff, 0.0), FVector2D(InnerOff, 0.0),
                    FVector2D(0.0, -InnerOff), FVector2D(0.0, InnerOff)};
                for (const FVector2D &Off : InnerOffsets)
                {
                    FSlateDrawElement::MakeText(
                        Elements,
                        Layer + LayerOffset + 1,
                        ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, FullClusterHeight),
                            FSlateLayoutTransform(FVector2f(Pos + Off))),
                        Text,
                        Font,
                        ESlateDrawEffect::None,
                        InnerHalo);
                }

                if (GlowIntensity >= 0.45f)
                {
                    const float OuterOff = 2.2f * Scale;
                    const FLinearColor OuterHalo = GlowColor.CopyWithNewOpacity(
                        FMath::Clamp(0.10f * GlowIntensity * GlowScale * Color.A, 0.0f, 0.24f));
                    const FVector2D OuterOffsets[4] = {
                        FVector2D(-OuterOff, -OuterOff), FVector2D(OuterOff, -OuterOff),
                        FVector2D(-OuterOff, OuterOff), FVector2D(OuterOff, OuterOff)};
                    for (const FVector2D &Off : OuterOffsets)
                    {
                        FSlateDrawElement::MakeText(
                            Elements,
                            Layer + LayerOffset + 1,
                            ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, FullClusterHeight),
                                FSlateLayoutTransform(FVector2f(Pos + Off))),
                            Text,
                            Font,
                            ESlateDrawEffect::None,
                            OuterHalo);
                    }
                }
            }

            FSlateDrawElement::MakeText(
                Elements,
                Layer + LayerOffset + 2,
                ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, FullClusterHeight), FSlateLayoutTransform(FVector2f(Pos))),
                Text,
                Font,
                ESlateDrawEffect::None,
                Color);
        };

        const auto DrawSilhouette = [&](const FSlateBrush *Brush, const FVector2D &BoxPos, const FVector2D &BoxExtent,
            float Opacity, bool bSquareWeaponTrim, float GlowIntensity, int32 LayerOffset)
        {
            if (!Brush || !Brush->GetResourceObject() || Opacity <= 0.01f)
            {
                return;
            }

            FVector2D FitPos;
            FVector2D FitExtent;
            FitAspectBox(BoxPos, BoxExtent, Brush->ImageSize, bSquareWeaponTrim, FitPos, FitExtent);
            if (FitExtent.X <= 0.5 || FitExtent.Y <= 0.5)
            {
                return;
            }

            const float ClipPad = 6.0f * Scale;
            const FGeometry ClipGeom = ClusterGeometry.MakeChild(
                FVector2f(BoxExtent + FVector2D(ClipPad * 2.0f, ClipPad * 2.0f)),
                FSlateLayoutTransform(FVector2f(BoxPos - FVector2D(ClipPad, ClipPad))));
            Elements.PushClip(FSlateClippingZone(ClipGeom));

            if (ShadowAlphaScale > 0.01f)
            {
                const FVector2D ShadowShift(1.3 * Scale, 1.6 * Scale);
                FSlateDrawElement::MakeBox(
                    Elements,
                    Layer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(FitExtent, FSlateLayoutTransform(FVector2f(FitPos + ShadowShift))),
                    Brush,
                    ESlateDrawEffect::NoPixelSnapping,
                    FLinearColor(0.0f, 0.0f, 0.0f, Opacity * 0.62f * ShadowAlphaScale));
            }

            if (GlowScale > 0.01f && GlowIntensity > 0.01f)
            {
                const float OuterExpand = 3.2f * Scale;
                const float InnerExpand = 1.4f * Scale;
                FSlateDrawElement::MakeBox(
                    Elements,
                    Layer + LayerOffset + 1,
                    ClusterGeometry.ToPaintGeometry(FitExtent + FVector2D(OuterExpand * 2.0f, OuterExpand * 2.0f),
                        FSlateLayoutTransform(FVector2f(FitPos - FVector2D(OuterExpand, OuterExpand)))),
                    Brush,
                    ESlateDrawEffect::NoPixelSnapping,
                    GlowColor.CopyWithNewOpacity(FMath::Clamp(0.13f * GlowIntensity * GlowScale * Opacity, 0.0f, 0.28f)));
                FSlateDrawElement::MakeBox(
                    Elements,
                    Layer + LayerOffset + 1,
                    ClusterGeometry.ToPaintGeometry(FitExtent + FVector2D(InnerExpand * 2.0f, InnerExpand * 2.0f),
                        FSlateLayoutTransform(FVector2f(FitPos - FVector2D(InnerExpand, InnerExpand)))),
                    Brush,
                    ESlateDrawEffect::NoPixelSnapping,
                    GlowColor.CopyWithNewOpacity(FMath::Clamp(0.22f * GlowIntensity * GlowScale * Opacity, 0.0f, 0.42f)));
            }

            FSlateDrawElement::MakeBox(
                Elements,
                Layer + LayerOffset + 2,
                ClusterGeometry.ToPaintGeometry(FitExtent, FSlateLayoutTransform(FVector2f(FitPos))),
                Brush,
                ESlateDrawEffect::NoPixelSnapping,
                AlphaTint(PrimaryTint, Opacity));

            Elements.PopClip();
        };

        // 1. 开放悬空式 SHD 左侧微发光脊线与背包导引微标
        const float VitalsStartY = TopReservedHeight;
        const float TopSpineStart = bShowWeaponBand ? 0.0f : VitalsStartY;
        const float TopSpineEnd = VitalsStartY + VitalsHeight;
        const float BottomSpineStart = TopSpineEnd + LoadoutGap * 0.55f;
        const float SpineAlpha = Snapshot.bLowHealth ? FMath::Lerp(0.48f, 1.0f, Snapshot.LowHealthPulse) : 0.96f;

        // 背包侧向微发光连接点与细导引线 始终锁定在常驻生命条中线 切枪零位移
        const double TetherY = VitalsStartY + HealthBarHeight * 0.5;
        DrawLine({FVector2D(-13.0 * Scale, TetherY + 2.0 * Scale), FVector2D(-3.5 * Scale, TetherY), FVector2D(0.0, TetherY)},
            1.15f * Scale, AlphaTint(PrimaryTint, 0.56f), 0, true, 0.70f);
        DrawRectGlow(FVector2D(-16.0 * Scale, TetherY + 0.5 * Scale), FVector2D(3.0 * Scale, 3.0 * Scale), 1.15f, 1);
        DrawRect(FVector2D(-16.0 * Scale, TetherY + 0.5 * Scale), FVector2D(3.0 * Scale, 3.0 * Scale),
            AlphaTint(PrimaryTint, 0.98f), 2);

        // 上半部主脊线 (持枪时向上延伸包裹武器区 空手时仅包裹常驻体征区)
        DrawRect(FVector2D(-0.8 * Scale, TopSpineStart - 0.8 * Scale),
            FVector2D(3.8 * Scale, (TopSpineEnd - TopSpineStart) + 1.6 * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, 0.48f * ShadowAlphaScale), 0);
        DrawRectGlow(FVector2D(0.0, TopSpineStart), FVector2D(2.2 * Scale, TopSpineEnd - TopSpineStart), SpineAlpha * 0.95f, 1);
        DrawRect(FVector2D(0.0, TopSpineStart), FVector2D(2.2 * Scale, TopSpineEnd - TopSpineStart),
            AlphaTint(PrimaryTint, SpineAlpha), 2);

        // 下半部常驻轻脊线 (武器槽与投掷物区)
        DrawRect(FVector2D(-0.5 * Scale, BottomSpineStart - 0.5 * Scale),
            FVector2D(2.5 * Scale, FullClusterHeight - BottomSpineStart + 1.0 * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, 0.36f * ShadowAlphaScale), 0);
        DrawRectGlow(FVector2D(0.0, BottomSpineStart), FVector2D(1.5 * Scale, FullClusterHeight - BottomSpineStart), 0.50f, 1);
        DrawRect(FVector2D(0.0, BottomSpineStart), FVector2D(1.5 * Scale, FullClusterHeight - BottomSpineStart),
            AlphaTint(PrimaryTint, 0.64f), 2);

        const float ContentLeft = 10.0f * Scale;
        const float ContentRight = ClusterWidth;
        const float ContentWidth = ContentRight - ContentLeft;

        const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

        // 2. 上方武器弹药区 (仅持武器时在常驻栏上方显现 已消除子弹条)
        if (bShowWeaponBand)
        {
            if (Snapshot.bHasAmmo)
            {
                const int32 LoadedFontSize = FMath::Max(14, FMath::RoundToInt(38.0f * Scale));
                const int32 CapFontSize = FMath::Max(10, FMath::RoundToInt(16.0f * Scale));
                const FSlateFontInfo LoadedFont(BoldComposite, LoadedFontSize);
                const FSlateFontInfo CapFont(LightComposite, CapFontSize);

                const bool bEmptyMag = Snapshot.LoadedAmmo <= 0;
                const bool bLowMag = !bEmptyMag && Snapshot.CapacityAmmo > 0
                    && Snapshot.LoadedAmmo <= FMath::Max(1, FMath::CeilToInt(Snapshot.CapacityAmmo * 0.25f));
                const float LoadedAlpha = bEmptyMag ? 0.50f : (bLowMag ? 0.80f + 0.20f * Snapshot.LowHealthPulse : 1.0f);

                const FVector2D LoadedExtent = FontMeasure->Measure(LoadedText, LoadedFont);
                const FVector2D CapExtent = FontMeasure->Measure(CapacityText, CapFont);
                const float LoadedTop = -5.0f * Scale;
                DrawTextLuminous(LoadedText, FVector2D(ContentLeft, LoadedTop), LoadedFont,
                    AlphaTint(PrimaryTint, LoadedAlpha), bEmptyMag ? 0.25f : 1.0f, 4);

                // 空弹匣极简微发光断框警示
                if (bEmptyMag)
                {
                    const float BoxW = FMath::Max(LoadedExtent.X + 8.0f * Scale, 30.0f * Scale);
                    const float BoxH = 36.0f * Scale;
                    const FLinearColor EmptyFrameTint = AlphaTint(PrimaryTint, 0.74f);
                    DrawLine({FVector2D(ContentLeft - 3.0f * Scale, 9.0f * Scale),
                        FVector2D(ContentLeft - 3.0f * Scale, 0.0f),
                        FVector2D(ContentLeft + 7.0f * Scale, 0.0f)},
                        1.2f * Scale, EmptyFrameTint, 3, true, 0.65f);
                    DrawLine({FVector2D(ContentLeft - 3.0f * Scale + BoxW - 8.0f * Scale, BoxH),
                        FVector2D(ContentLeft - 3.0f * Scale + BoxW, BoxH),
                        FVector2D(ContentLeft - 3.0f * Scale + BoxW, BoxH - 8.0f * Scale)},
                        1.2f * Scale, EmptyFrameTint, 3, true, 0.65f);
                }

                const float CapX = ContentLeft + LoadedExtent.X + 4.0f * Scale;
                const float CapY = LoadedTop + LoadedExtent.Y - CapExtent.Y - 6.0f * Scale;
                DrawTextLuminous(CapacityText, FVector2D(CapX, CapY), CapFont,
                    AlphaTint(SecondaryTint, 0.96f), 0.50f, 4);

                // 换弹时原位旋转微发光双弧 弹药数字零位移
                if (Snapshot.bReloading)
                {
                    const FVector2D RingCenter(CapX + CapExtent.X + 14.0f * Scale, CapY + CapExtent.Y * 0.52f);
                    const float RingRadius = 7.0f * Scale;
                    const auto DrawArc = [&](float StartDeg, float SweepDeg, float Thick, const FLinearColor &ArcColor, float ArcGlow)
                    {
                        TArray<FVector2D> ArcPts;
                        const int32 Steps = FMath::Max(6, FMath::CeilToInt(SweepDeg / 18.0f));
                        ArcPts.Reserve(Steps + 1);
                        for (int32 Step = 0; Step <= Steps; ++Step)
                        {
                            const float Rad = FMath::DegreesToRadians(StartDeg + SweepDeg * Step / Steps);
                            ArcPts.Add(RingCenter + FVector2D(FMath::Cos(Rad), FMath::Sin(Rad)) * RingRadius);
                        }
                        DrawLine(ArcPts, Thick, ArcColor, 4, true, ArcGlow);
                    };

                    DrawArc(0.0f, 360.0f, 1.2f * Scale, AlphaTint(PrimaryTint, 0.28f), 0.2f);
                    DrawArc(Snapshot.ReloadAngle, 135.0f, 2.0f * Scale, AlphaTint(PrimaryTint, 0.98f), 1.0f);
                }
            }

            // 右侧饱满微发光主武器纯白剪影
            const float WeaponBoxWidth = 114.0f * Scale;
            const float WeaponBoxHeight = 38.0f * Scale;
            const float WeaponBoxLeft = ContentRight - WeaponBoxWidth;
            const float WeaponBoxTop = 1.0f * Scale;
            if (ActiveWeaponBrush)
            {
                DrawSilhouette(ActiveWeaponBrush, FVector2D(WeaponBoxLeft, WeaponBoxTop),
                    FVector2D(WeaponBoxWidth, WeaponBoxHeight), 0.98f, true, 0.90f, 4);
            }
        }

        // 3. 常驻体征层 全境封锁 2 标志性微发光分段瓷白生命条 + 极细自适应体力轨
        float CursorY = VitalsStartY;
        const int32 Segments = FMath::Clamp(HealthSegments, 1, 8);
        const float SegmentGap = 3.5f * Scale;
        const float SegmentWidth = (ContentWidth - SegmentGap * (Segments - 1)) / Segments;
        const float HealthPulseAlpha = Snapshot.bLowHealth
            ? FMath::Lerp(0.58f, 1.0f, Snapshot.LowHealthPulse)
            : 0.98f;

        for (int32 SegIndex = 0; SegIndex < Segments; ++SegIndex)
        {
            const float SegStart = static_cast<float>(SegIndex) / Segments;
            const float SegEnd = static_cast<float>(SegIndex + 1) / Segments;
            const float SegX = ContentLeft + SegIndex * (SegmentWidth + SegmentGap);

            // 锐利暗色凹槽衬底 让纯白分段在亮场景下依然立体清晰
            DrawRect(FVector2D(SegX - 1.1f * Scale, CursorY - 1.1f * Scale),
                FVector2D(SegmentWidth + 2.2f * Scale, HealthBarHeight + 2.2f * Scale),
                FLinearColor(0.0f, 0.0f, 0.0f, 0.56f * ShadowAlphaScale), 0);

            const float HealthFill = FMath::Clamp((Snapshot.HealthFraction - SegStart) / (SegEnd - SegStart), 0.0f, 1.0f);
            if (HealthFill > 0.001f)
            {
                DrawRectGlow(FVector2D(SegX, CursorY), FVector2D(SegmentWidth * HealthFill, HealthBarHeight),
                    HealthPulseAlpha * 0.90f, 1);
            }

            // 空槽霜白底轨
            DrawRect(FVector2D(SegX, CursorY), FVector2D(SegmentWidth, HealthBarHeight),
                AlphaTint(PrimaryTint, 0.20f), 2);

            // 受击延迟白色残影
            const float TrailFill = FMath::Clamp((Snapshot.DamageTrailFraction - SegStart) / (SegEnd - SegStart), 0.0f, 1.0f);
            if (TrailFill > 0.001f)
            {
                DrawRect(FVector2D(SegX, CursorY), FVector2D(SegmentWidth * TrailFill, HealthBarHeight),
                    AlphaTint(PrimaryTint, 0.50f), 3);
            }

            // 当前生命实心填充 倒地态切换为空心脉冲细框
            if (HealthFill > 0.001f)
            {
                const float FillW = SegmentWidth * HealthFill;
                if (Snapshot.bDowned)
                {
                    DrawRect(FVector2D(SegX, CursorY), FVector2D(FillW, HealthBarHeight),
                        AlphaTint(PrimaryTint, 0.28f * HealthPulseAlpha), 4);
                    DrawLine({FVector2D(SegX, CursorY), FVector2D(SegX + FillW, CursorY),
                        FVector2D(SegX + FillW, CursorY + HealthBarHeight), FVector2D(SegX, CursorY + HealthBarHeight),
                        FVector2D(SegX, CursorY)},
                        1.3f * Scale, AlphaTint(PrimaryTint, HealthPulseAlpha), 4, false, 0.7f);
                }
                else
                {
                    DrawRect(FVector2D(SegX, CursorY), FVector2D(FillW, HealthBarHeight),
                        AlphaTint(PrimaryTint, HealthPulseAlpha), 4);
                }
            }
        }

        // 低血量左端微发光几何警示菱形
        if (Snapshot.bLowHealth)
        {
            const float DiamondR = 3.8f * Scale;
            const FVector2D DiamondC(1.1f * Scale, CursorY + HealthBarHeight * 0.5f);
            DrawLine({
                DiamondC + FVector2D(0.0, -DiamondR),
                DiamondC + FVector2D(DiamondR, 0.0),
                DiamondC + FVector2D(0.0, DiamondR),
                DiamondC + FVector2D(-DiamondR, 0.0),
                DiamondC + FVector2D(0.0, -DiamondR)},
                1.4f * Scale, AlphaTint(PrimaryTint, HealthPulseAlpha), 5, true, 1.1f);
        }

        // 紧贴血条下方的极细微发光体力轨道
        const float StaminaY = CursorY + HealthBarHeight + StaminaGap;
        const float StaminaVis = FMath::Clamp(Snapshot.StaminaOpacity, 0.22f, 1.0f);
        const float StaminaFillW = ContentWidth * FMath::Clamp(Snapshot.StaminaFraction, 0.0f, 1.0f);
        DrawRect(FVector2D(ContentLeft - 0.8f * Scale, StaminaY - 0.8f * Scale),
            FVector2D(ContentWidth + 1.6f * Scale, StaminaHeight + 1.6f * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, 0.42f * StaminaVis * ShadowAlphaScale), 0);
        if (StaminaFillW > 0.5f)
        {
            DrawRectGlow(FVector2D(ContentLeft, StaminaY), FVector2D(StaminaFillW, StaminaHeight), StaminaVis * 0.65f, 1);
        }
        DrawRect(FVector2D(ContentLeft, StaminaY), FVector2D(ContentWidth, StaminaHeight),
            AlphaTint(SecondaryTint, 0.24f * StaminaVis), 2);
        DrawRect(FVector2D(ContentLeft, StaminaY), FVector2D(StaminaFillW, StaminaHeight),
            AlphaTint(PrimaryTint, 0.94f * StaminaVis), 3);

        CursorY += VitalsHeight + LoadoutGap;

        // 4. 常驻下层 武器槽 (1/2/3) 与投掷物微区
        const float WeaponZoneWidth = bHasThrowables ? 150.0f * Scale : ContentWidth;
        const float SlotGap = 5.0f * Scale;
        const float SlotWidth = (WeaponZoneWidth - SlotGap * (FBBBDivisionHudSnapshot::MaxWeaponSlots - 1)) / FBBBDivisionHudSnapshot::MaxWeaponSlots;
        const int32 SlotNumSize = FMath::Max(8, FMath::RoundToInt(10.0f * Scale));
        const FSlateFontInfo SlotNumFont(BoldComposite, SlotNumSize);

        for (int32 SlotIndex = 0; SlotIndex < FBBBDivisionHudSnapshot::MaxWeaponSlots; ++SlotIndex)
        {
            const FBBBDivisionHudSlotSnapshot &Slot = Snapshot.WeaponSlots[SlotIndex];
            const float SlotX = ContentLeft + SlotIndex * (SlotWidth + SlotGap);

            const float SlotAlpha = Slot.bActive ? 0.98f : (Slot.bOccupied ? 0.52f : 0.18f);
            const float BottomBarH = Slot.bActive ? 2.2f * Scale : 1.0f * Scale;
            const float BottomBarAlpha = Slot.bActive ? 0.98f : (Slot.bOccupied ? 0.30f : 0.14f);

            DrawRect(FVector2D(SlotX - 0.5f * Scale, CursorY + LoadoutHeight - BottomBarH - 0.5f * Scale),
                FVector2D(SlotWidth + 1.0f * Scale, BottomBarH + 1.0f * Scale),
                FLinearColor(0.0f, 0.0f, 0.0f, 0.42f * BottomBarAlpha * ShadowAlphaScale), 0);
            if (Slot.bActive)
            {
                DrawRectGlow(FVector2D(SlotX, CursorY + LoadoutHeight - BottomBarH),
                    FVector2D(SlotWidth, BottomBarH), 0.95f, 1);
            }
            DrawRect(FVector2D(SlotX, CursorY + LoadoutHeight - BottomBarH), FVector2D(SlotWidth, BottomBarH),
                AlphaTint(PrimaryTint, BottomBarAlpha), 2);

            // 左侧中线对齐的极小槽号 1 / 2 / 3
            const FString KeyDigit = FString::FromInt(SlotIndex + 1);
            DrawTextLuminous(KeyDigit, FVector2D(SlotX + 1.0f * Scale, CursorY + 6.0f * Scale),
                SlotNumFont, AlphaTint(PrimaryTint, Slot.bActive ? 0.98f : (Slot.bOccupied ? 0.68f : 0.30f)),
                Slot.bActive ? 0.65f : 0.15f, 4);

            // 饱满迷你武器剪影
            if (Slot.bOccupied && WeaponSlotBrushes[SlotIndex])
            {
                DrawSilhouette(WeaponSlotBrushes[SlotIndex],
                    FVector2D(SlotX + 11.5f * Scale, CursorY + 2.0f * Scale),
                    FVector2D(SlotWidth - 12.5f * Scale, LoadoutHeight - 6.0f * Scale),
                    SlotAlpha, true, Slot.bActive ? 0.70f : 0.15f, 4);
            }
        }

        // 右侧投掷物与备用消耗品区
        if (bHasThrowables)
        {
            const float DividerX = ContentLeft + WeaponZoneWidth + 6.0f * Scale;
            DrawRect(FVector2D(DividerX, CursorY + 3.0f * Scale), FVector2D(1.0f * Scale, LoadoutHeight - 6.0f * Scale),
                AlphaTint(PrimaryTint, 0.36f), 3);

            float ThrowX = DividerX + 7.0f * Scale;
            const int32 ThrowNumSize = FMath::Max(10, FMath::RoundToInt(14.0f * Scale));
            const int32 SpareNumSize = FMath::Max(8, FMath::RoundToInt(10.0f * Scale));
            const FSlateFontInfo ThrowNumFont(BoldComposite, ThrowNumSize);
            const FSlateFontInfo SpareNumFont(LightComposite, SpareNumSize);

            if (Snapshot.SelectedThrowable.bOccupied)
            {
                const float SelectedW = 40.0f * Scale;
                DrawRectGlow(FVector2D(ThrowX, CursorY + LoadoutHeight - 2.0f * Scale),
                    FVector2D(SelectedW, 2.0f * Scale), 0.75f, 0);
                DrawRect(FVector2D(ThrowX, CursorY + LoadoutHeight - 2.0f * Scale), FVector2D(SelectedW, 2.0f * Scale),
                    AlphaTint(PrimaryTint, 0.92f), 3);

                if (SelectedThrowableBrush)
                {
                    DrawSilhouette(SelectedThrowableBrush,
                        FVector2D(ThrowX + 1.0f * Scale, CursorY + 1.0f * Scale),
                        FVector2D(20.0f * Scale, LoadoutHeight - 6.0f * Scale),
                        0.96f, false, 0.70f, 4);
                }

                if (!SelectedThrowableText.IsEmpty())
                {
                    const FVector2D NumExtent = FontMeasure->Measure(SelectedThrowableText, ThrowNumFont);
                    DrawTextLuminous(SelectedThrowableText,
                        FVector2D(ThrowX + 24.0f * Scale, CursorY + (LoadoutHeight - NumExtent.Y) * 0.42f),
                        ThrowNumFont, AlphaTint(PrimaryTint, 0.98f), 0.65f, 5);
                }

                ThrowX += SelectedW + 6.0f * Scale;
            }

            // 备用投掷物微标 图标与数字左右分离 绝不重叠
            const float SpareW = 24.0f * Scale;
            const float SpareGap = 4.0f * Scale;
            for (int32 SpareIdx = 0; SpareIdx < FBBBDivisionHudSnapshot::MaxSpareThrowables; ++SpareIdx)
            {
                if (!Snapshot.SpareThrowables[SpareIdx].bOccupied)
                {
                    continue;
                }

                const float CellX = ThrowX + SpareIdx * (SpareW + SpareGap);
                if (CellX + SpareW > ContentRight + 4.0f * Scale)
                {
                    break;
                }

                if (SpareThrowableBrushes[SpareIdx])
                {
                    DrawSilhouette(SpareThrowableBrushes[SpareIdx],
                        FVector2D(CellX, CursorY + 3.0f * Scale),
                        FVector2D(13.0f * Scale, LoadoutHeight - 9.0f * Scale),
                        0.54f, false, 0.15f, 4);
                }

                if (!SpareThrowableTexts[SpareIdx].IsEmpty())
                {
                    const FVector2D TinyExtent = FontMeasure->Measure(SpareThrowableTexts[SpareIdx], SpareNumFont);
                    DrawTextLuminous(SpareThrowableTexts[SpareIdx],
                        FVector2D(CellX + 14.0f * Scale, CursorY + LoadoutHeight - TinyExtent.Y - 3.0f * Scale),
                        SpareNumFont, AlphaTint(SecondaryTint, 0.84f), 0.15f, 5);
                }
            }
        }

        return Layer + 8;
    }

    FBBBDivisionHudSnapshot Snapshot;
    FLinearColor PrimaryTint = FLinearColor(1.0f, 1.0f, 1.0f, 0.98f);
    FLinearColor SecondaryTint = FLinearColor(0.92f, 0.95f, 0.99f, 0.84f);
    float ContrastShadowStrength = 0.78f;
    float EmissiveGlowStrength = 1.0f;
    FVector2D AnchorPixelOffset = FVector2D(18.0, 26.0);
    int32 HealthSegments = 5;
    float PerspectiveSlopeY = -0.115f;
    float PerspectiveShearX = -0.055f;

    const FSlateBrush *ActiveWeaponBrush = nullptr;
    const FSlateBrush *WeaponSlotBrushes[FBBBDivisionHudSnapshot::MaxWeaponSlots] = {};
    const FSlateBrush *SelectedThrowableBrush = nullptr;
    const FSlateBrush *SpareThrowableBrushes[FBBBDivisionHudSnapshot::MaxSpareThrowables] = {};

private:
    TWeakObjectPtr<UFontFace> CachedBoldFace;
    TWeakObjectPtr<UFontFace> CachedLightFace;
    TSharedPtr<const FCompositeFont> BoldComposite;
    TSharedPtr<const FCompositeFont> LightComposite;

    int32 CachedLoaded = INDEX_NONE;
    int32 CachedCapacity = INDEX_NONE;
    int32 CachedSelectedThrowableCount = INDEX_NONE;
    int32 CachedSpareThrowableCounts[FBBBDivisionHudSnapshot::MaxSpareThrowables] = {INDEX_NONE, INDEX_NONE};

    FString LoadedText;
    FString CapacityText;
    FString SelectedThrowableText;
    FString SpareThrowableTexts[FBBBDivisionHudSnapshot::MaxSpareThrowables];
};

//--------------

UBBBDivisionBackpackHud::UBBBDivisionBackpackHud(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(
        TEXT("/Game/_Project/Characters/Items/Shared/M_HudItemSilhouette.M_HudItemSilhouette"));
    SilhouetteMaterial = MaterialFinder.Object;

    static ConstructorHelpers::FObjectFinder<UFontFace> BoldFinder(
        TEXT("/Game/_ThirdParty/Fonts/Akrobat/Akrobat_Bold.Akrobat_Bold"));
    BoldNumberFont = BoldFinder.Object;

    static ConstructorHelpers::FObjectFinder<UFontFace> LightFinder(
        TEXT("/Game/_ThirdParty/Fonts/Akrobat/Akrobat_Regular.Akrobat_Regular"));
    LightNumberFont = LightFinder.Object;
}

TSharedRef<SWidget> UBBBDivisionBackpackHud::RebuildWidget()
{
    SlateWidget = SNew(SBBBDivisionBackpackHud);
    SynchronizeProperties();
    return SlateWidget.ToSharedRef();
}

void UBBBDivisionBackpackHud::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    if (!SlateWidget)
    {
        return;
    }

    SlateWidget->PrimaryTint = PrimaryTint;
    SlateWidget->SecondaryTint = SecondaryTint;
    SlateWidget->ContrastShadowStrength = ContrastShadowStrength;
    SlateWidget->EmissiveGlowStrength = EmissiveGlowStrength;
    SlateWidget->HealthSegments = HealthSegments;
    SlateWidget->PerspectiveSlopeY = PerspectiveSlopeY;
    SlateWidget->PerspectiveShearX = PerspectiveShearX;
    SlateWidget->AnchorPixelOffset = AnchorPixelOffset;
    SlateWidget->ApplyFonts(BoldNumberFont, LightNumberFont);
    SlateWidget->Invalidate(EInvalidateWidgetReason::Paint);
}

void UBBBDivisionBackpackHud::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    SlateWidget.Reset();
}

const FSlateBrush *UBBBDivisionBackpackHud::UpdateSilhouetteSlot(int32 PoolIndex, UTexture2D *Texture)
{
    if (PoolIndex < 0 || PoolIndex >= SilhouettePoolCount)
    {
        return nullptr;
    }

    if (SilhouetteMaterials.Num() != SilhouettePoolCount)
    {
        SilhouetteMaterials.SetNum(SilhouettePoolCount);
    }

    if (CachedTextures.Num() != SilhouettePoolCount)
    {
        CachedTextures.SetNum(SilhouettePoolCount);
    }

    FSlateBrush &Brush = SilhouetteBrushes[PoolIndex];
    if (!Texture)
    {
        CachedTextures[PoolIndex] = nullptr;
        Brush.SetResourceObject(nullptr);
        Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
        return nullptr;
    }

    if (CachedTextures[PoolIndex] != Texture || !Brush.GetResourceObject())
    {
        CachedTextures[PoolIndex] = Texture;
        if (SilhouetteMaterial)
        {
            if (!SilhouetteMaterials[PoolIndex])
            {
                SilhouetteMaterials[PoolIndex] = UMaterialInstanceDynamic::Create(SilhouetteMaterial, this);
            }

            if (UMaterialInstanceDynamic *Material = SilhouetteMaterials[PoolIndex])
            {
                Material->SetTextureParameterValue(TEXT("ImageTexture"), Texture);
                Brush.SetResourceObject(Material);
            }
        }
        else
        {
            Brush.SetResourceObject(Texture);
        }

        Brush.DrawAs = ESlateBrushDrawType::Image;
        Brush.ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
    }

    return &Brush;
}

void UBBBDivisionBackpackHud::SyncSilhouetteMaterials(const FBBBDivisionHudSnapshot &InSnapshot)
{
    if (!SlateWidget)
    {
        return;
    }

    int32 PoolIndex = 0;
    SlateWidget->ActiveWeaponBrush = UpdateSilhouetteSlot(PoolIndex++, InSnapshot.ActiveWeaponIcon);

    for (int32 SlotIndex = 0; SlotIndex < FBBBDivisionHudSnapshot::MaxWeaponSlots; ++SlotIndex)
    {
        SlateWidget->WeaponSlotBrushes[SlotIndex] = UpdateSilhouetteSlot(PoolIndex++, InSnapshot.WeaponSlots[SlotIndex].Icon);
    }

    SlateWidget->SelectedThrowableBrush = UpdateSilhouetteSlot(PoolIndex++, InSnapshot.SelectedThrowable.Icon);

    for (int32 SpareIndex = 0; SpareIndex < FBBBDivisionHudSnapshot::MaxSpareThrowables; ++SpareIndex)
    {
        SlateWidget->SpareThrowableBrushes[SpareIndex] = UpdateSilhouetteSlot(PoolIndex++, InSnapshot.SpareThrowables[SpareIndex].Icon);
    }
}

void UBBBDivisionBackpackHud::SetSnapshot(const FBBBDivisionHudSnapshot &InSnapshot)
{
    bool bChanged = !bHasSnapshot
        || LastSnapshot.bHasAnchor != InSnapshot.bHasAnchor
        || LastSnapshot.bHasAmmo != InSnapshot.bHasAmmo
        || LastSnapshot.bReloading != InSnapshot.bReloading
        || LastSnapshot.bLowHealth != InSnapshot.bLowHealth
        || LastSnapshot.bDowned != InSnapshot.bDowned
        || LastSnapshot.bHasActiveWeapon != InSnapshot.bHasActiveWeapon
        || LastSnapshot.LoadedAmmo != InSnapshot.LoadedAmmo
        || LastSnapshot.CapacityAmmo != InSnapshot.CapacityAmmo
        || LastSnapshot.ActiveWeaponIcon != InSnapshot.ActiveWeaponIcon
        || LastSnapshot.SelectedThrowable != InSnapshot.SelectedThrowable
        || !FMath::IsNearlyEqual(LastSnapshot.HealthFraction, InSnapshot.HealthFraction, 0.001f)
        || !FMath::IsNearlyEqual(LastSnapshot.DamageTrailFraction, InSnapshot.DamageTrailFraction, 0.001f)
        || !FMath::IsNearlyEqual(LastSnapshot.StaminaFraction, InSnapshot.StaminaFraction, 0.001f)
        || !FMath::IsNearlyEqual(LastSnapshot.StaminaOpacity, InSnapshot.StaminaOpacity, 0.004f)
        || !FMath::IsNearlyEqual(LastSnapshot.LowHealthPulse, InSnapshot.LowHealthPulse, 0.008f)
        || !FMath::IsNearlyEqual(LastSnapshot.DistanceScale, InSnapshot.DistanceScale, 0.002f)
        || (InSnapshot.bReloading && !FMath::IsNearlyEqual(LastSnapshot.ReloadAngle, InSnapshot.ReloadAngle, 1.0f))
        || !LastSnapshot.AnchorPosition.Equals(InSnapshot.AnchorPosition, 0.25)
        || !LastSnapshot.ViewportSize.Equals(InSnapshot.ViewportSize, 0.5);

    if (!bChanged)
    {
        for (int32 Index = 0; Index < FBBBDivisionHudSnapshot::MaxWeaponSlots; ++Index)
        {
            if (LastSnapshot.WeaponSlots[Index] != InSnapshot.WeaponSlots[Index])
            {
                bChanged = true;
                break;
            }
        }
    }

    if (!bChanged)
    {
        for (int32 Index = 0; Index < FBBBDivisionHudSnapshot::MaxSpareThrowables; ++Index)
        {
            if (LastSnapshot.SpareThrowables[Index] != InSnapshot.SpareThrowables[Index])
            {
                bChanged = true;
                break;
            }
        }
    }

    if (!bChanged)
    {
        return;
    }

    LastSnapshot = InSnapshot;
    bHasSnapshot = true;
    if (!SlateWidget)
    {
        return;
    }

    SyncSilhouetteMaterials(InSnapshot);
    SlateWidget->Snapshot = InSnapshot;
    SlateWidget->RefreshCachedNumbers();
    SlateWidget->Invalidate(EInvalidateWidgetReason::Paint);
}

bool UBBBDivisionBackpackHud::IsAmmoShown() const
{
    return bHasSnapshot && LastSnapshot.bHasAmmo;
}

int32 UBBBDivisionBackpackHud::GetDisplayedLoadedAmmo() const
{
    return bHasSnapshot ? LastSnapshot.LoadedAmmo : 0;
}

int32 UBBBDivisionBackpackHud::GetDisplayedCapacityAmmo() const
{
    return bHasSnapshot ? LastSnapshot.CapacityAmmo : 0;
}

FVector2D UBBBDivisionBackpackHud::GetSmoothedAnchorPosition() const
{
    return bHasSnapshot ? LastSnapshot.AnchorPosition : FVector2D::ZeroVector;
}
