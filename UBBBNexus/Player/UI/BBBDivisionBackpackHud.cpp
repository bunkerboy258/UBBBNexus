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

/** 全境封锁 2 风格开放悬空极简白色背包侧边战术集群绘制器 */
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
        const float MasterOpacity = FMath::Lerp(1.0f, AimDimOpacity, Snapshot.AimBlend);
        if (MasterOpacity <= 0.01f)
        {
            return Layer;
        }

        const bool bShowAmmoBand = Snapshot.bHasAmmo;
        const bool bHasThrowables = Snapshot.SelectedThrowable.bOccupied
            || Snapshot.SpareThrowables[0].bOccupied
            || Snapshot.SpareThrowables[1].bOccupied;

        // 统一 268px 宽度 保证空手与持枪切换时底部武器与投掷物位置稳定不跳动
        const float ClusterWidth = 268.0f * Scale;
        const float AmmoBandHeight = bShowAmmoBand ? 54.0f * Scale : 0.0f;
        const float AmmoGap = bShowAmmoBand ? 8.0f * Scale : 0.0f;
        const float HealthBarHeight = 9.5f * Scale;
        const float StaminaGap = 4.5f * Scale;
        const float StaminaHeight = 2.8f * Scale;
        const float VitalsHeight = HealthBarHeight + StaminaGap + StaminaHeight;
        const float LoadoutGap = 11.0f * Scale;
        const float LoadoutHeight = 28.0f * Scale;
        const float ClusterHeight = AmmoBandHeight + AmmoGap + VitalsHeight + LoadoutGap + LoadoutHeight;

        // 屏幕空间平滑定位 避让视口边缘与中央准星禁区
        FVector2D Anchor = Snapshot.bHasAnchor
            ? Snapshot.AnchorPosition
            : FVector2D(LocalSize.X * 0.54, LocalSize.Y * 0.62);
        Anchor += AnchorPixelOffset * Scale;
        Anchor += FVector2D(22.0 * Scale, 30.0 * Scale) * Snapshot.AimBlend;

        const double SafeEdgeX = 28.0 * ViewportScale;
        const double SafeEdgeY = 28.0 * ViewportScale;
        const double ReticleAvoidLeft = LocalSize.X * 0.5 + 52.0 * ViewportScale;
        const double ReticleAvoidTop = LocalSize.Y * 0.5 - 56.0 * ViewportScale;
        const double ReticleAvoidBottom = LocalSize.Y * 0.5 + 56.0 * ViewportScale;

        FVector2D Origin(Anchor.X, Anchor.Y - ClusterHeight * 0.38);
        if (Origin.Y < ReticleAvoidBottom && (Origin.Y + ClusterHeight) > ReticleAvoidTop && Origin.X < ReticleAvoidLeft)
        {
            Origin.X = FMath::Max(Origin.X, ReticleAvoidLeft);
        }

        Origin.X = FMath::Clamp(Origin.X, SafeEdgeX, FMath::Max(SafeEdgeX, LocalSize.X - ClusterWidth - SafeEdgeX));
        Origin.Y = FMath::Clamp(Origin.Y, SafeEdgeY, FMath::Max(SafeEdgeY, LocalSize.Y - ClusterHeight - SafeEdgeY));

        // 极轻 2.5D 透视微斜 保留全息悬浮感同时避免水平细线锯齿
        const FMatrix2x2 PerspectiveMatrix(1.0f, PerspectiveSlopeY, PerspectiveShearX, 1.0f);
        const FSlateRenderTransform PerspectiveTransform(PerspectiveMatrix);
        const FGeometry ClusterGeometry = Geometry.MakeChild(
            FVector2f(ClusterWidth, ClusterHeight),
            FSlateLayoutTransform(FVector2f(Origin)),
            PerspectiveTransform,
            FVector2f(0.0f, 0.5f));

        static const FSlateColorBrush SolidBrush(FLinearColor::White);
        const float ShadowAlphaScale = FMath::Clamp(ContrastShadowStrength, 0.0f, 1.0f);
        const auto AlphaTint = [MasterOpacity](const FLinearColor &Base, float ExtraAlpha = 1.0f)
        {
            return Base.CopyWithNewOpacity(Base.A * MasterOpacity * ExtraAlpha);
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

        const auto DrawLine = [&](const TArray<FVector2D> &Points, float Thickness, const FLinearColor &Color, int32 LayerOffset, bool bShadow = true)
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

            FSlateDrawElement::MakeLines(
                Elements,
                Layer + LayerOffset + 1,
                ClusterGeometry.ToPaintGeometry(),
                Points,
                ESlateDrawEffect::NoPixelSnapping,
                Color,
                true,
                Thickness);
        };

        // 双重微影绘制纯白数字 无需灰色矩形背板即可在强光天空下保持刀锋级清晰度
        const auto DrawTextCrisp = [&](const FString &Text, const FVector2D &Pos, const FSlateFontInfo &Font,
            const FLinearColor &Color, int32 LayerOffset)
        {
            if (Text.IsEmpty() || Color.A <= 0.002f)
            {
                return;
            }

            if (ShadowAlphaScale > 0.01f)
            {
                const float SAlpha = FMath::Max(Color.A, 0.78f * MasterOpacity) * ShadowAlphaScale;
                FSlateDrawElement::MakeText(
                    Elements,
                    Layer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, ClusterHeight),
                        FSlateLayoutTransform(FVector2f(Pos + FVector2D(0.0, 1.4 * Scale)))),
                    Text,
                    Font,
                    ESlateDrawEffect::None,
                    FLinearColor(0.0f, 0.0f, 0.0f, SAlpha * 0.80f));

                FSlateDrawElement::MakeText(
                    Elements,
                    Layer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, ClusterHeight),
                        FSlateLayoutTransform(FVector2f(Pos + FVector2D(1.2 * Scale, 0.9 * Scale)))),
                    Text,
                    Font,
                    ESlateDrawEffect::None,
                    FLinearColor(0.0f, 0.0f, 0.0f, SAlpha * 0.58f));
            }

            FSlateDrawElement::MakeText(
                Elements,
                Layer + LayerOffset + 1,
                ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, ClusterHeight), FSlateLayoutTransform(FVector2f(Pos))),
                Text,
                Font,
                ESlateDrawEffect::None,
                Color);
        };

        const auto DrawSilhouette = [&](const FSlateBrush *Brush, const FVector2D &BoxPos, const FVector2D &BoxExtent,
            float Opacity, bool bSquareWeaponTrim, int32 LayerOffset)
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

            const FGeometry ClipGeom = ClusterGeometry.MakeChild(
                FVector2f(BoxExtent + FVector2D(4.0 * Scale, 4.0 * Scale)),
                FSlateLayoutTransform(FVector2f(BoxPos - FVector2D(2.0 * Scale, 2.0 * Scale))));
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
                    FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * Opacity * 0.62f * ShadowAlphaScale));
            }

            FSlateDrawElement::MakeBox(
                Elements,
                Layer + LayerOffset + 1,
                ClusterGeometry.ToPaintGeometry(FitExtent, FSlateLayoutTransform(FVector2f(FitPos))),
                Brush,
                ESlateDrawEffect::NoPixelSnapping,
                AlphaTint(PrimaryTint, Opacity));

            Elements.PopClip();
        };

        // 1. 开放悬空式 SHD 左侧脊线与背包导引微标 彻底摒弃沉闷的灰色矩形框
        const float TopSpineEnd = AmmoBandHeight + AmmoGap + VitalsHeight;
        const float BottomSpineStart = TopSpineEnd + LoadoutGap * 0.55f;
        const float SpineAlpha = Snapshot.bLowHealth ? FMath::Lerp(0.45f, 1.0f, Snapshot.LowHealthPulse) : 0.92f;

        // 背包侧向微连接点与细导引线
        const double TetherY = bShowAmmoBand ? (AmmoBandHeight + AmmoGap + HealthBarHeight * 0.5) : (HealthBarHeight * 0.5);
        DrawLine({FVector2D(-13.0 * Scale, TetherY + 2.0 * Scale), FVector2D(-3.5 * Scale, TetherY), FVector2D(0.0, TetherY)},
            1.1f * Scale, AlphaTint(PrimaryTint, 0.46f), 0, true);
        DrawRect(FVector2D(-16.0 * Scale, TetherY + 0.5 * Scale), FVector2D(3.0 * Scale, 3.0 * Scale),
            AlphaTint(PrimaryTint, 0.85f), 2);

        // 上半部主脊线 (弹药与生命体征区)
        DrawRect(FVector2D(-0.8 * Scale, -0.8 * Scale), FVector2D(3.8 * Scale, TopSpineEnd + 1.6 * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * 0.48f * ShadowAlphaScale), 1);
        DrawRect(FVector2D(0.0, 0.0), FVector2D(2.2 * Scale, TopSpineEnd), AlphaTint(PrimaryTint, SpineAlpha), 2);

        // 下半部轻脊线 (武器槽与投掷物区)
        DrawRect(FVector2D(-0.5 * Scale, BottomSpineStart - 0.5 * Scale),
            FVector2D(2.5 * Scale, ClusterHeight - BottomSpineStart + 1.0 * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * 0.36f * ShadowAlphaScale), 1);
        DrawRect(FVector2D(0.0, BottomSpineStart), FVector2D(1.5 * Scale, ClusterHeight - BottomSpineStart),
            AlphaTint(PrimaryTint, 0.55f), 2);

        const float ContentLeft = 10.0f * Scale;
        const float ContentRight = ClusterWidth;
        const float ContentWidth = ContentRight - ContentLeft;
        float CursorY = 0.0f;

        const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

        // 2. 顶部主弹药与饱满主武器剪影区
        if (bShowAmmoBand)
        {
            const int32 LoadedFontSize = FMath::Max(14, FMath::RoundToInt(40.0f * Scale));
            const int32 CapFontSize = FMath::Max(10, FMath::RoundToInt(16.0f * Scale));
            const FSlateFontInfo LoadedFont(BoldComposite, LoadedFontSize);
            const FSlateFontInfo CapFont(LightComposite, CapFontSize);

            const bool bEmptyMag = Snapshot.LoadedAmmo <= 0;
            const bool bLowMag = !bEmptyMag && Snapshot.CapacityAmmo > 0
                && Snapshot.LoadedAmmo <= FMath::Max(1, FMath::CeilToInt(Snapshot.CapacityAmmo * 0.25f));
            const float LoadedAlpha = bEmptyMag ? 0.46f : (bLowMag ? 0.78f + 0.22f * Snapshot.LowHealthPulse : 1.0f);

            const FVector2D LoadedExtent = FontMeasure->Measure(LoadedText, LoadedFont);
            const FVector2D CapExtent = FontMeasure->Measure(CapacityText, CapFont);
            const float LoadedTop = CursorY - 6.0f * Scale;
            DrawTextCrisp(LoadedText, FVector2D(ContentLeft, LoadedTop), LoadedFont, AlphaTint(PrimaryTint, LoadedAlpha), 4);

            // 空弹匣极简断框警示
            if (bEmptyMag)
            {
                const float BoxW = FMath::Max(LoadedExtent.X + 8.0f * Scale, 30.0f * Scale);
                const float BoxH = 38.0f * Scale;
                const FLinearColor EmptyFrameTint = AlphaTint(PrimaryTint, 0.68f);
                DrawLine({FVector2D(ContentLeft - 3.0f * Scale, CursorY + 9.0f * Scale),
                    FVector2D(ContentLeft - 3.0f * Scale, CursorY),
                    FVector2D(ContentLeft + 7.0f * Scale, CursorY)},
                    1.2f * Scale, EmptyFrameTint, 3, true);
                DrawLine({FVector2D(ContentLeft - 3.0f * Scale + BoxW - 8.0f * Scale, CursorY + BoxH),
                    FVector2D(ContentLeft - 3.0f * Scale + BoxW, CursorY + BoxH),
                    FVector2D(ContentLeft - 3.0f * Scale + BoxW, CursorY + BoxH - 8.0f * Scale)},
                    1.2f * Scale, EmptyFrameTint, 3, true);
            }

            const float CapX = ContentLeft + LoadedExtent.X + 4.0f * Scale;
            const float CapY = LoadedTop + LoadedExtent.Y - CapExtent.Y - 6.0f * Scale;
            DrawTextCrisp(CapacityText, FVector2D(CapX, CapY), CapFont, AlphaTint(SecondaryTint, 0.96f), 4);

            // 换弹时原位旋转双弧 弹药数字零位移
            if (Snapshot.bReloading)
            {
                const FVector2D RingCenter(CapX + CapExtent.X + 14.0f * Scale, CapY + CapExtent.Y * 0.52f);
                const float RingRadius = 7.0f * Scale;
                const auto DrawArc = [&](float StartDeg, float SweepDeg, float Thick, const FLinearColor &ArcColor)
                {
                    TArray<FVector2D> ArcPts;
                    const int32 Steps = FMath::Max(6, FMath::CeilToInt(SweepDeg / 18.0f));
                    ArcPts.Reserve(Steps + 1);
                    for (int32 Step = 0; Step <= Steps; ++Step)
                    {
                        const float Rad = FMath::DegreesToRadians(StartDeg + SweepDeg * Step / Steps);
                        ArcPts.Add(RingCenter + FVector2D(FMath::Cos(Rad), FMath::Sin(Rad)) * RingRadius);
                    }
                    DrawLine(ArcPts, Thick, ArcColor, 4, true);
                };

                DrawArc(0.0f, 360.0f, 1.2f * Scale, AlphaTint(PrimaryTint, 0.28f));
                DrawArc(Snapshot.ReloadAngle, 135.0f, 2.0f * Scale, AlphaTint(PrimaryTint, 0.98f));
            }

            // 右侧饱满主武器纯白剪影 (按实心枪体区域缩放 视觉分量与弹药大数字平衡)
            const float WeaponBoxWidth = 114.0f * Scale;
            const float WeaponBoxHeight = 40.0f * Scale;
            const float WeaponBoxLeft = ContentRight - WeaponBoxWidth;
            const float WeaponBoxTop = CursorY + 2.0f * Scale;
            if (ActiveWeaponBrush)
            {
                DrawSilhouette(ActiveWeaponBrush, FVector2D(WeaponBoxLeft, WeaponBoxTop),
                    FVector2D(WeaponBoxWidth, WeaponBoxHeight), 0.96f, true, 4);
            }

            // 弹药底缘精密弹药微轨 (霰弹枪呈现独立弹壳砖块 步枪呈现连续轨带四分位刻痕)
            const float RailY = CursorY + AmmoBandHeight - 3.5f * Scale;
            const float RailHeight = 3.2f * Scale;
            const float AmmoRatio = Snapshot.CapacityAmmo > 0
                ? FMath::Clamp(static_cast<float>(Snapshot.LoadedAmmo) / Snapshot.CapacityAmmo, 0.0f, 1.0f)
                : 0.0f;

            if (!Snapshot.bAmmoContinuous && Snapshot.CapacityAmmo > 0 && Snapshot.CapacityAmmo <= 12)
            {
                const float SegGap = 2.8f * Scale;
                const float SegW = (ContentWidth - SegGap * (Snapshot.CapacityAmmo - 1)) / Snapshot.CapacityAmmo;
                for (int32 Index = 0; Index < Snapshot.CapacityAmmo; ++Index)
                {
                    const float SegX = ContentLeft + Index * (SegW + SegGap);
                    const bool bFilled = Index < Snapshot.LoadedAmmo;
                    DrawRect(FVector2D(SegX - 0.8f * Scale, RailY - 0.8f * Scale),
                        FVector2D(SegW + 1.6f * Scale, RailHeight + 1.6f * Scale),
                        FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * 0.45f * ShadowAlphaScale), 2);
                    DrawRect(FVector2D(SegX, RailY), FVector2D(SegW, RailHeight),
                        AlphaTint(PrimaryTint, bFilled ? 0.96f : 0.20f), 4);
                }
            }
            else
            {
                DrawRect(FVector2D(ContentLeft - 0.8f * Scale, RailY - 0.8f * Scale),
                    FVector2D(ContentWidth + 1.6f * Scale, RailHeight + 1.6f * Scale),
                    FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * 0.45f * ShadowAlphaScale), 2);
                DrawRect(FVector2D(ContentLeft, RailY), FVector2D(ContentWidth, RailHeight),
                    AlphaTint(PrimaryTint, 0.20f), 3);
                DrawRect(FVector2D(ContentLeft, RailY), FVector2D(ContentWidth * AmmoRatio, RailHeight),
                    AlphaTint(PrimaryTint, 0.96f), 4);

                // 四分位精密微刻痕 增强战术仪器质感
                for (int32 Tick = 1; Tick <= 3; ++Tick)
                {
                    const float TickX = ContentLeft + ContentWidth * (Tick * 0.25f);
                    DrawRect(FVector2D(TickX, RailY), FVector2D(1.0f * Scale, RailHeight),
                        FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * 0.42f), 5);
                }
            }

            CursorY += AmmoBandHeight + AmmoGap;
        }

        // 3. 中层 全境封锁 2 标志性分段瓷白生命条 + 极细自适应体力轨
        const int32 Segments = FMath::Clamp(HealthSegments, 1, 8);
        const float SegmentGap = 3.5f * Scale;
        const float SegmentWidth = (ContentWidth - SegmentGap * (Segments - 1)) / Segments;
        const float HealthPulseAlpha = Snapshot.bLowHealth
            ? FMath::Lerp(0.56f, 1.0f, Snapshot.LowHealthPulse)
            : 0.97f;

        for (int32 SegIndex = 0; SegIndex < Segments; ++SegIndex)
        {
            const float SegStart = static_cast<float>(SegIndex) / Segments;
            const float SegEnd = static_cast<float>(SegIndex + 1) / Segments;
            const float SegX = ContentLeft + SegIndex * (SegmentWidth + SegmentGap);

            // 锐利暗色凹槽衬底 让纯白分段如瓷砖般悬空立体
            DrawRect(FVector2D(SegX - 1.1f * Scale, CursorY - 1.1f * Scale),
                FVector2D(SegmentWidth + 2.2f * Scale, HealthBarHeight + 2.2f * Scale),
                FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * 0.56f * ShadowAlphaScale), 2);

            // 空槽霜白底轨
            DrawRect(FVector2D(SegX, CursorY), FVector2D(SegmentWidth, HealthBarHeight),
                AlphaTint(PrimaryTint, 0.19f), 3);

            // 受击延迟白色残影
            const float TrailFill = FMath::Clamp((Snapshot.DamageTrailFraction - SegStart) / (SegEnd - SegStart), 0.0f, 1.0f);
            if (TrailFill > 0.001f)
            {
                DrawRect(FVector2D(SegX, CursorY), FVector2D(SegmentWidth * TrailFill, HealthBarHeight),
                    AlphaTint(PrimaryTint, 0.48f), 4);
            }

            // 当前生命实心填充 倒地态切换为空心脉冲细框
            const float HealthFill = FMath::Clamp((Snapshot.HealthFraction - SegStart) / (SegEnd - SegStart), 0.0f, 1.0f);
            if (HealthFill > 0.001f)
            {
                const float FillW = SegmentWidth * HealthFill;
                if (Snapshot.bDowned)
                {
                    DrawRect(FVector2D(SegX, CursorY), FVector2D(FillW, HealthBarHeight),
                        AlphaTint(PrimaryTint, 0.28f * HealthPulseAlpha), 5);
                    DrawLine({FVector2D(SegX, CursorY), FVector2D(SegX + FillW, CursorY),
                        FVector2D(SegX + FillW, CursorY + HealthBarHeight), FVector2D(SegX, CursorY + HealthBarHeight),
                        FVector2D(SegX, CursorY)},
                        1.3f * Scale, AlphaTint(PrimaryTint, HealthPulseAlpha), 5, false);
                }
                else
                {
                    DrawRect(FVector2D(SegX, CursorY), FVector2D(FillW, HealthBarHeight),
                        AlphaTint(PrimaryTint, HealthPulseAlpha), 5);
                }
            }
        }

        // 低血量左端几何警示菱形
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
                1.4f * Scale, AlphaTint(PrimaryTint, HealthPulseAlpha), 6, true);
        }

        // 紧贴血条下方的极细体力轨道 带微暗托底确保亮天空可读
        const float StaminaY = CursorY + HealthBarHeight + StaminaGap;
        const float StaminaVis = FMath::Clamp(Snapshot.StaminaOpacity, 0.20f, 1.0f);
        DrawRect(FVector2D(ContentLeft - 0.8f * Scale, StaminaY - 0.8f * Scale),
            FVector2D(ContentWidth + 1.6f * Scale, StaminaHeight + 1.6f * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * 0.42f * StaminaVis * ShadowAlphaScale), 2);
        DrawRect(FVector2D(ContentLeft, StaminaY), FVector2D(ContentWidth, StaminaHeight),
            AlphaTint(SecondaryTint, 0.24f * StaminaVis), 3);
        DrawRect(FVector2D(ContentLeft, StaminaY), FVector2D(ContentWidth * FMath::Clamp(Snapshot.StaminaFraction, 0.0f, 1.0f), StaminaHeight),
            AlphaTint(PrimaryTint, 0.94f * StaminaVis), 4);

        CursorY += VitalsHeight + LoadoutGap;

        // 4. 下层 清晰分离的武器槽 (1/2/3) 与投掷物微区
        const float WeaponZoneWidth = bHasThrowables ? 150.0f * Scale : ContentWidth;
        const float SlotGap = 5.0f * Scale;
        const float SlotWidth = (WeaponZoneWidth - SlotGap * (FBBBDivisionHudSnapshot::MaxWeaponSlots - 1)) / FBBBDivisionHudSnapshot::MaxWeaponSlots;
        const int32 SlotNumSize = FMath::Max(8, FMath::RoundToInt(10.0f * Scale));
        const FSlateFontInfo SlotNumFont(BoldComposite, SlotNumSize);

        for (int32 SlotIndex = 0; SlotIndex < FBBBDivisionHudSnapshot::MaxWeaponSlots; ++SlotIndex)
        {
            const FBBBDivisionHudSlotSnapshot &Slot = Snapshot.WeaponSlots[SlotIndex];
            const float SlotX = ContentLeft + SlotIndex * (SlotWidth + SlotGap);

            const float SlotAlpha = Slot.bActive ? 0.96f : (Slot.bOccupied ? 0.50f : 0.18f);
            const float BottomBarH = Slot.bActive ? 2.2f * Scale : 1.0f * Scale;
            const float BottomBarAlpha = Slot.bActive ? 0.96f : (Slot.bOccupied ? 0.30f : 0.14f);

            // 底缘细线带微暗阴影
            DrawRect(FVector2D(SlotX - 0.5f * Scale, CursorY + LoadoutHeight - BottomBarH - 0.5f * Scale),
                FVector2D(SlotWidth + 1.0f * Scale, BottomBarH + 1.0f * Scale),
                FLinearColor(0.0f, 0.0f, 0.0f, MasterOpacity * 0.42f * BottomBarAlpha * ShadowAlphaScale), 2);
            DrawRect(FVector2D(SlotX, CursorY + LoadoutHeight - BottomBarH), FVector2D(SlotWidth, BottomBarH),
                AlphaTint(PrimaryTint, BottomBarAlpha), 3);

            // 左侧中线对齐的极小槽号 1 / 2 / 3 与枪托保留清晰呼吸间距
            const FString KeyDigit = FString::FromInt(SlotIndex + 1);
            DrawTextCrisp(KeyDigit, FVector2D(SlotX + 1.0f * Scale, CursorY + 6.0f * Scale),
                SlotNumFont, AlphaTint(PrimaryTint, Slot.bActive ? 0.98f : (Slot.bOccupied ? 0.68f : 0.30f)), 4);

            // 饱满迷你武器剪影 (启用正方形枪体裁剪缩放 清晰辨识每把武器)
            if (Slot.bOccupied && WeaponSlotBrushes[SlotIndex])
            {
                DrawSilhouette(WeaponSlotBrushes[SlotIndex],
                    FVector2D(SlotX + 11.5f * Scale, CursorY + 2.0f * Scale),
                    FVector2D(SlotWidth - 12.5f * Scale, LoadoutHeight - 6.0f * Scale),
                    SlotAlpha, true, 4);
            }
        }

        // 右侧投掷物与备用消耗品区 严格保证间距零重叠
        if (bHasThrowables)
        {
            const float DividerX = ContentLeft + WeaponZoneWidth + 6.0f * Scale;
            DrawRect(FVector2D(DividerX, CursorY + 3.0f * Scale), FVector2D(1.0f * Scale, LoadoutHeight - 6.0f * Scale),
                AlphaTint(PrimaryTint, 0.34f), 3);

            float ThrowX = DividerX + 7.0f * Scale;
            const int32 ThrowNumSize = FMath::Max(10, FMath::RoundToInt(14.0f * Scale));
            const int32 SpareNumSize = FMath::Max(8, FMath::RoundToInt(10.0f * Scale));
            const FSlateFontInfo ThrowNumFont(BoldComposite, ThrowNumSize);
            const FSlateFontInfo SpareNumFont(LightComposite, SpareNumSize);

            if (Snapshot.SelectedThrowable.bOccupied)
            {
                const float SelectedW = 40.0f * Scale;
                DrawRect(FVector2D(ThrowX, CursorY + LoadoutHeight - 2.0f * Scale), FVector2D(SelectedW, 2.0f * Scale),
                    AlphaTint(PrimaryTint, 0.90f), 3);

                if (SelectedThrowableBrush)
                {
                    DrawSilhouette(SelectedThrowableBrush,
                        FVector2D(ThrowX + 1.0f * Scale, CursorY + 1.0f * Scale),
                        FVector2D(20.0f * Scale, LoadoutHeight - 6.0f * Scale),
                        0.96f, false, 4);
                }

                if (!SelectedThrowableText.IsEmpty())
                {
                    const FVector2D NumExtent = FontMeasure->Measure(SelectedThrowableText, ThrowNumFont);
                    DrawTextCrisp(SelectedThrowableText,
                        FVector2D(ThrowX + 24.0f * Scale, CursorY + (LoadoutHeight - NumExtent.Y) * 0.42f),
                        ThrowNumFont, AlphaTint(PrimaryTint, 0.98f), 5);
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
                        0.52f, false, 4);
                }

                if (!SpareThrowableTexts[SpareIdx].IsEmpty())
                {
                    const FVector2D TinyExtent = FontMeasure->Measure(SpareThrowableTexts[SpareIdx], SpareNumFont);
                    DrawTextCrisp(SpareThrowableTexts[SpareIdx],
                        FVector2D(CellX + 14.0f * Scale, CursorY + LoadoutHeight - TinyExtent.Y - 3.0f * Scale),
                        SpareNumFont, AlphaTint(SecondaryTint, 0.82f), 5);
                }
            }
        }

        return Layer + 8;
    }

    FBBBDivisionHudSnapshot Snapshot;
    FLinearColor PrimaryTint = FLinearColor(0.97f, 0.98f, 1.0f, 0.96f);
    FLinearColor SecondaryTint = FLinearColor(0.90f, 0.93f, 0.97f, 0.82f);
    float ContrastShadowStrength = 0.80f;
    FVector2D AnchorPixelOffset = FVector2D(18.0, 22.0);
    int32 HealthSegments = 5;
    float PerspectiveSlopeY = -0.014f;
    float PerspectiveShearX = -0.012f;
    float AimDimOpacity = 0.42f;

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
    SlateWidget->HealthSegments = HealthSegments;
    SlateWidget->PerspectiveSlopeY = PerspectiveSlopeY;
    SlateWidget->PerspectiveShearX = PerspectiveShearX;
    SlateWidget->AnchorPixelOffset = AnchorPixelOffset;
    SlateWidget->AimDimOpacity = AimDimOpacity;
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
        || LastSnapshot.bAmmoContinuous != InSnapshot.bAmmoContinuous
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
        || !FMath::IsNearlyEqual(LastSnapshot.AimBlend, InSnapshot.AimBlend, 0.004f)
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
