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

/** 全境封锁 2 风格开放悬空极简白色无倾斜无发光背包侧边战术集群绘制器 */
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
        const float BaseScale = ViewportScale * Snapshot.DistanceScale;
        const float Scale = BaseScale * FMath::Clamp(HudScale, 0.5f, 2.5f);

        static const FSlateColorBrush SolidBrush(FLinearColor::White);
        const float ShadowAlphaScale = FMath::Clamp(ContrastShadowStrength, 0.0f, 1.0f);
        const auto AlphaTint = [](const FLinearColor &Base, float ExtraAlpha = 1.0f)
        {
            return Base.CopyWithNewOpacity(Base.A * ExtraAlpha);
        };

        // 0. 底层批量绘制其他玩家头顶同款缩小护甲+血量条 (严格低于本机背包 HUD 层级 零 RT 低开销)
        const int32 Segments = FMath::Clamp(ArmorSegments, 1, 8);
        for (const FBBBDivisionOverheadEntry &Entry : Snapshot.TeammateOverheads)
        {
            if (Entry.Alpha <= 0.01f)
            {
                continue;
            }

            const float MiniScale = ViewportScale * Entry.DistanceScale;
            const float MiniWidth = 92.0f * MiniScale;
            const float MiniArmorH = Entry.bHasArmorPool ? 5.0f * MiniScale : 0.0f;
            const float MiniGap = Entry.bHasArmorPool ? 2.0f * MiniScale : 0.0f;
            const float MiniHealthH = 3.0f * MiniScale;
            const float MiniTotalH = MiniArmorH + MiniGap + MiniHealthH;
            const FVector2D MiniOrigin(Entry.ScreenPosition.X - MiniWidth * 0.5, Entry.ScreenPosition.Y - MiniTotalH);
            const float EAlpha = Entry.Alpha;

            const auto DrawMiniRect = [&](const FVector2D &Pos, const FVector2D &Extent, const FLinearColor &Color, int32 LayerOffset)
            {
                if (Extent.X <= 0.0 || Extent.Y <= 0.0 || Color.A <= 0.002f)
                {
                    return;
                }

                FSlateDrawElement::MakeBox(
                    Elements,
                    Layer + LayerOffset,
                    Geometry.ToPaintGeometry(Extent, FSlateLayoutTransform(FVector2f(MiniOrigin + Pos))),
                    &SolidBrush,
                    ESlateDrawEffect::NoPixelSnapping,
                    Color);
            };

            const FLinearColor MiniAccentTint = Entry.bDowned ? DownedHealthTint : PrimaryTint;
            // 极细左侧 SHD 脊线微标
            DrawMiniRect(FVector2D(-4.0f * MiniScale, 0.0), FVector2D(1.4f * MiniScale, MiniTotalH),
                AlphaTint(MiniAccentTint, 0.88f * EAlpha), 1);

            float MiniY = 0.0f;
            if (Entry.bHasArmorPool)
            {
                const float MiniSegGap = 2.0f * MiniScale;
                const float MiniSegW = (MiniWidth - MiniSegGap * (Segments - 1)) / Segments;
                const float MiniArmorDim = Entry.bDowned ? 0.35f : 1.0f;
                for (int32 SegIndex = 0; SegIndex < Segments; ++SegIndex)
                {
                    const float SegStart = static_cast<float>(SegIndex) / Segments;
                    const float SegEnd = static_cast<float>(SegIndex + 1) / Segments;
                    const float SegX = SegIndex * (MiniSegW + MiniSegGap);

                    DrawMiniRect(FVector2D(SegX - 0.8f * MiniScale, MiniY - 0.8f * MiniScale),
                        FVector2D(MiniSegW + 1.6f * MiniScale, MiniArmorH + 1.6f * MiniScale),
                        FLinearColor(0.0f, 0.0f, 0.0f, 0.54f * ShadowAlphaScale * EAlpha * MiniArmorDim), 0);
                    DrawMiniRect(FVector2D(SegX, MiniY), FVector2D(MiniSegW, MiniArmorH),
                        AlphaTint(PrimaryTint, 0.18f * EAlpha * MiniArmorDim), 1);

                    const float ArmorFill = FMath::Clamp((Entry.ArmorFraction - SegStart) / (SegEnd - SegStart), 0.0f, 1.0f);
                    if (ArmorFill > 0.001f)
                    {
                        DrawMiniRect(FVector2D(SegX, MiniY), FVector2D(MiniSegW * ArmorFill, MiniArmorH),
                            AlphaTint(PrimaryTint, 0.96f * EAlpha), 2);
                    }
                }
                MiniY += MiniArmorH + MiniGap;
            }

            // 紧贴护甲下方的连续血量条 倒地态切换为战术红
            const FLinearColor MiniHealthBase = Entry.bDowned
                ? DownedHealthTint
                : (Entry.bLowHealth
                    ? FMath::Lerp(FLinearColor(0.98f, 0.42f, 0.38f, 0.96f), DownedHealthTint, Snapshot.LowHealthPulse)
                    : FLinearColor(0.94f, 0.96f, 0.98f, 0.94f));
            const float MiniHealthPulse = Entry.bDowned ? FMath::Lerp(0.76f, 1.0f, Snapshot.LowHealthPulse) : 1.0f;
            const FLinearColor MiniHealthTrack = Entry.bDowned
                ? FLinearColor(0.42f, 0.08f, 0.08f, 0.48f * EAlpha)
                : AlphaTint(PrimaryTint, 0.18f * EAlpha);

            DrawMiniRect(FVector2D(-0.8f * MiniScale, MiniY - 0.8f * MiniScale),
                FVector2D(MiniWidth + 1.6f * MiniScale, MiniHealthH + 1.6f * MiniScale),
                FLinearColor(0.0f, 0.0f, 0.0f, 0.58f * ShadowAlphaScale * EAlpha), 0);
            DrawMiniRect(FVector2D(0.0, MiniY), FVector2D(MiniWidth, MiniHealthH), MiniHealthTrack, 1);

            const float MiniHealthFillW = MiniWidth * FMath::Clamp(Entry.HealthFraction, 0.0f, 1.0f);
            if (MiniHealthFillW > 0.25f)
            {
                DrawMiniRect(FVector2D(0.0, MiniY), FVector2D(MiniHealthFillW, MiniHealthH),
                    AlphaTint(MiniHealthBase, MiniHealthPulse * EAlpha), 2);
            }
        }

        const int32 LocalBaseLayer = Layer + 5;

        const bool bShowWeaponBand = Snapshot.bHasAmmo || Snapshot.bHasActiveWeapon;
        const bool bHasThrowables = Snapshot.SelectedThrowable.bOccupied
            || Snapshot.SpareThrowables[0].bOccupied
            || Snapshot.SpareThrowables[1].bOccupied;

        // 常驻护甲/血量/装备栏固定锚定 武器弹药区固定预留于其上方 切枪与空手时下方常驻栏零位移
        const float ClusterWidth = 268.0f * Scale;
        const float WeaponBandHeight = 40.0f * Scale;
        const float WeaponGap = 9.0f * Scale;
        const float TopReservedHeight = WeaponBandHeight + WeaponGap;
        const float ArmorBarHeight = Snapshot.bHasArmorPool ? 9.0f * Scale : 0.0f;
        const float HealthGap = Snapshot.bHasArmorPool ? 2.8f * Scale : 0.0f;
        const float HealthBarHeight = 4.8f * Scale;
        const float StaminaGap = 6.5f * Scale;
        const float StaminaHeight = 2.0f * Scale;
        const float VitalsHeight = ArmorBarHeight + HealthGap + HealthBarHeight + StaminaGap + StaminaHeight;
        const float LoadoutGap = 9.5f * Scale;
        const float LoadoutHeight = 28.0f * Scale;
        const float PersistentHeight = VitalsHeight + LoadoutGap + LoadoutHeight;
        const float FullClusterHeight = TopReservedHeight + PersistentHeight;

        // 屏幕空间锚点锁定在常驻体征栏基准线 瞄准与空手均不减弱显示也不挤压常驻栏
        FVector2D Anchor = Snapshot.bHasAnchor
            ? Snapshot.AnchorPosition
            : FVector2D(LocalSize.X * 0.54, LocalSize.Y * 0.62);
        Anchor += AnchorPixelOffset * BaseScale;

        const double SafeEdgeX = 28.0 * ViewportScale;
        const double SafeEdgeY = 28.0 * ViewportScale;
        const double ReticleCenterY = LocalSize.Y * 0.5;
        const double ReticleAvoidLeft = LocalSize.X * 0.5 + 48.0 * ViewportScale;
        const double ReticleHalfH = 56.0 * ViewportScale;
        const double BlendMarginY = 96.0 * ViewportScale;

        FVector2D Origin(Anchor.X, Anchor.Y - TopReservedHeight);
        // 平滑避让中央准星区域 防止瞄准蹲伏穿过边界时单帧跳变与闪烁
        const double TopClearance = (ReticleCenterY + ReticleHalfH) - Origin.Y;
        const double BottomClearance = (Origin.Y + FullClusterHeight) - (ReticleCenterY - ReticleHalfH);
        const double VerticalOverlap = FMath::Min(TopClearance, BottomClearance);
        const double AvoidAlpha = FMath::SmoothStep(0.0, 1.0,
            FMath::Clamp((VerticalOverlap + BlendMarginY) / (BlendMarginY * 1.5), 0.0, 1.0));
        if (Origin.X < ReticleAvoidLeft && AvoidAlpha > 0.0)
        {
            Origin.X = FMath::Lerp(Origin.X, ReticleAvoidLeft, AvoidAlpha);
        }

        Origin.X = FMath::Clamp(Origin.X, SafeEdgeX, FMath::Max(SafeEdgeX, LocalSize.X - ClusterWidth - SafeEdgeX));
        Origin.Y = FMath::Clamp(Origin.Y, SafeEdgeY, FMath::Max(SafeEdgeY, LocalSize.Y - FullClusterHeight - SafeEdgeY));

        const FGeometry ClusterGeometry = Geometry.MakeChild(
            FVector2f(ClusterWidth, FullClusterHeight),
            FSlateLayoutTransform(FVector2f(Origin)));

        const auto DrawRect = [&](const FVector2D &Pos, const FVector2D &Extent, const FLinearColor &Color, int32 LayerOffset)
        {
            if (Extent.X <= 0.0 || Extent.Y <= 0.0 || Color.A <= 0.002f)
            {
                return;
            }

            FSlateDrawElement::MakeBox(
                Elements,
                LocalBaseLayer + LayerOffset,
                ClusterGeometry.ToPaintGeometry(Extent, FSlateLayoutTransform(FVector2f(Pos))),
                &SolidBrush,
                ESlateDrawEffect::NoPixelSnapping,
                Color);
        };

        const auto DrawLine = [&](const TArray<FVector2D> &Points, float Thickness, const FLinearColor &Color,
            int32 LayerOffset, bool bShadow = true)
        {
            if (Points.Num() < 2 || Color.A <= 0.002f)
            {
                return;
            }

            if (bShadow && ShadowAlphaScale > 0.01f)
            {
                FSlateDrawElement::MakeLines(
                    Elements,
                    LocalBaseLayer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(),
                    Points,
                    ESlateDrawEffect::NoPixelSnapping,
                    FLinearColor(0.0f, 0.0f, 0.0f, Color.A * 0.65f * ShadowAlphaScale),
                    true,
                    Thickness + 1.8f * Scale);
            }

            FSlateDrawElement::MakeLines(
                Elements,
                LocalBaseLayer + LayerOffset + 1,
                ClusterGeometry.ToPaintGeometry(),
                Points,
                ESlateDrawEffect::NoPixelSnapping,
                Color,
                true,
                Thickness);
        };

        // 暗影保底 + 锐利纯白前景 绘制无自发光数字
        const auto DrawTextCrisp = [&](const FString &Text, const FVector2D &Pos, const FSlateFontInfo &Font,
            const FLinearColor &Color, int32 LayerOffset)
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
                    LocalBaseLayer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, FullClusterHeight),
                        FSlateLayoutTransform(FVector2f(Pos + FVector2D(0.0, 1.4 * Scale)))),
                    Text,
                    Font,
                    ESlateDrawEffect::None,
                    FLinearColor(0.0f, 0.0f, 0.0f, SAlpha * 0.78f));

                FSlateDrawElement::MakeText(
                    Elements,
                    LocalBaseLayer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, FullClusterHeight),
                        FSlateLayoutTransform(FVector2f(Pos + FVector2D(1.2 * Scale, 0.9 * Scale)))),
                    Text,
                    Font,
                    ESlateDrawEffect::None,
                    FLinearColor(0.0f, 0.0f, 0.0f, SAlpha * 0.55f));
            }

            FSlateDrawElement::MakeText(
                Elements,
                LocalBaseLayer + LayerOffset + 1,
                ClusterGeometry.ToPaintGeometry(FVector2D(ClusterWidth, FullClusterHeight),
                    FSlateLayoutTransform(FVector2f(Pos))),
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
                    LocalBaseLayer + LayerOffset,
                    ClusterGeometry.ToPaintGeometry(FitExtent, FSlateLayoutTransform(FVector2f(FitPos + ShadowShift))),
                    Brush,
                    ESlateDrawEffect::NoPixelSnapping,
                    FLinearColor(0.0f, 0.0f, 0.0f, Opacity * 0.62f * ShadowAlphaScale));
            }

            FSlateDrawElement::MakeBox(
                Elements,
                LocalBaseLayer + LayerOffset + 1,
                ClusterGeometry.ToPaintGeometry(FitExtent, FSlateLayoutTransform(FVector2f(FitPos))),
                Brush,
                ESlateDrawEffect::NoPixelSnapping,
                AlphaTint(PrimaryTint, Opacity));

            Elements.PopClip();
        };

        // 1. 开放悬空式 SHD 左侧纯白脊线与细导引线 (倒地时主脊线转战术红)
        const float VitalsStartY = TopReservedHeight;
        const float TopSpineStart = bShowWeaponBand ? 0.0f : VitalsStartY;
        const float TopSpineEnd = VitalsStartY + ArmorBarHeight + HealthGap + HealthBarHeight;
        const float BottomSpineStart = VitalsStartY + VitalsHeight + LoadoutGap * 0.55f;
        const float SpineAlpha = (Snapshot.bLowHealth || Snapshot.bDowned)
            ? FMath::Lerp(0.55f, 1.0f, Snapshot.LowHealthPulse)
            : 0.96f;
        const FLinearColor TopSpineTint = Snapshot.bDowned ? DownedHealthTint : PrimaryTint;

        // 背包侧向细导引线 锁定在常驻体征区中心 切枪零位移
        const double TetherY = VitalsStartY + (ArmorBarHeight > 0.0f ? ArmorBarHeight * 0.5 : HealthBarHeight * 0.5);
        DrawLine({FVector2D(-13.0 * Scale, TetherY + 2.0 * Scale), FVector2D(-3.5 * Scale, TetherY), FVector2D(0.0, TetherY)},
            1.15f * Scale, AlphaTint(TopSpineTint, 0.56f), 0, true);

        // 上半部主脊线 (持武器时向上延伸包裹武器区 空手时仅包裹常驻护甲与血量区)
        DrawRect(FVector2D(-0.8 * Scale, TopSpineStart - 0.8 * Scale),
            FVector2D(3.8 * Scale, (TopSpineEnd - TopSpineStart) + 1.6 * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, 0.48f * ShadowAlphaScale), 0);
        DrawRect(FVector2D(0.0, TopSpineStart), FVector2D(2.2 * Scale, TopSpineEnd - TopSpineStart),
            AlphaTint(TopSpineTint, SpineAlpha), 1);

        // 下半部常驻轻脊线 (武器槽与投掷物区)
        DrawRect(FVector2D(-0.5 * Scale, BottomSpineStart - 0.5 * Scale),
            FVector2D(2.5 * Scale, FullClusterHeight - BottomSpineStart + 1.0 * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, 0.36f * ShadowAlphaScale), 0);
        DrawRect(FVector2D(0.0, BottomSpineStart), FVector2D(1.5 * Scale, FullClusterHeight - BottomSpineStart),
            AlphaTint(PrimaryTint, 0.64f), 1);

        const float ContentLeft = 10.0f * Scale;
        const float ContentRight = ClusterWidth;
        const float ContentWidth = ContentRight - ContentLeft;

        const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

        // 2. 上方武器弹药区 (仅持武器时在常驻栏上方显现 无子弹条)
        if (bShowWeaponBand)
        {
            if (Snapshot.bHasAmmo)
            {
                const int32 LoadedFontSize = FMath::Max(14, FMath::RoundToInt(35.0f * Scale));
                const int32 CapFontSize = FMath::Max(10, FMath::RoundToInt(16.0f * Scale));
                const FSlateFontInfo LoadedFont(BoldComposite, LoadedFontSize);
                const FSlateFontInfo CapFont(LightComposite, CapFontSize);

                const bool bEmptyMag = Snapshot.LoadedAmmo <= 0;
                const bool bLowMag = !bEmptyMag && Snapshot.CapacityAmmo > 0
                    && Snapshot.LoadedAmmo <= FMath::Max(1, FMath::CeilToInt(Snapshot.CapacityAmmo * 0.25f));
                const float LoadedAlpha = bEmptyMag ? 0.50f : (bLowMag ? 0.80f + 0.20f * Snapshot.LowHealthPulse : 1.0f);

                const FVector2D LoadedExtent = FontMeasure->Measure(LoadedText, LoadedFont);
                const FVector2D CapExtent = FontMeasure->Measure(CapacityText, CapFont);
                // 抵消字体顶部内置留白 使主弹药数字与主脊线顶端平齐并在下方留出呼吸空隙不遮挡血条
                const float LoadedTop = -14.0f * Scale;
                DrawTextCrisp(LoadedText, FVector2D(ContentLeft, LoadedTop), LoadedFont,
                    AlphaTint(PrimaryTint, LoadedAlpha), 3);

                // 空弹匣极简断框警示
                if (bEmptyMag)
                {
                    const float BoxW = FMath::Max(LoadedExtent.X + 8.0f * Scale, 30.0f * Scale);
                    const float BoxH = 36.0f * Scale;
                    const FLinearColor EmptyFrameTint = AlphaTint(PrimaryTint, 0.74f);
                    DrawLine({FVector2D(ContentLeft - 3.0f * Scale, 9.0f * Scale),
                        FVector2D(ContentLeft - 3.0f * Scale, 0.0f),
                        FVector2D(ContentLeft + 7.0f * Scale, 0.0f)},
                        1.2f * Scale, EmptyFrameTint, 2, true);
                    DrawLine({FVector2D(ContentLeft - 3.0f * Scale + BoxW - 8.0f * Scale, BoxH),
                        FVector2D(ContentLeft - 3.0f * Scale + BoxW, BoxH),
                        FVector2D(ContentLeft - 3.0f * Scale + BoxW, BoxH - 8.0f * Scale)},
                        1.2f * Scale, EmptyFrameTint, 2, true);
                }

                const float CapX = ContentLeft + LoadedExtent.X + 4.0f * Scale;
                const float CapY = LoadedTop + LoadedExtent.Y * 0.80f - CapExtent.Y * 0.80f;
                DrawTextCrisp(CapacityText, FVector2D(CapX, CapY), CapFont,
                    AlphaTint(SecondaryTint, 0.96f), 3);

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
                        DrawLine(ArcPts, Thick, ArcColor, 3, true);
                    };

                    DrawArc(0.0f, 360.0f, 1.2f * Scale, AlphaTint(PrimaryTint, 0.28f));
                    DrawArc(Snapshot.ReloadAngle, 135.0f, 2.0f * Scale, AlphaTint(PrimaryTint, 0.98f));
                }
            }

            // 右侧饱满主武器纯白剪影
            const float WeaponBoxWidth = 114.0f * Scale;
            const float WeaponBoxHeight = 38.0f * Scale;
            const float WeaponBoxLeft = ContentRight - WeaponBoxWidth;
            const float WeaponBoxTop = 1.0f * Scale;
            if (ActiveWeaponBrush)
            {
                DrawSilhouette(ActiveWeaponBrush, FVector2D(WeaponBoxLeft, WeaponBoxTop),
                    FVector2D(WeaponBoxWidth, WeaponBoxHeight), 0.98f, true, 3);
            }
        }

        // 3. 常驻体征层 上方 5 段纯白护甲 + 紧贴下方连续生命条 (倒地战术红) + 间距分离的按需体力细轨
        float CursorY = VitalsStartY;
        if (Snapshot.bHasArmorPool)
        {
            const float SegmentGap = 3.5f * Scale;
            const float SegmentWidth = (ContentWidth - SegmentGap * (Segments - 1)) / Segments;
            const float ArmorDim = Snapshot.bDowned ? 0.36f : 1.0f;

            for (int32 SegIndex = 0; SegIndex < Segments; ++SegIndex)
            {
                const float SegStart = static_cast<float>(SegIndex) / Segments;
                const float SegEnd = static_cast<float>(SegIndex + 1) / Segments;
                const float SegX = ContentLeft + SegIndex * (SegmentWidth + SegmentGap);

                // 锐利暗色凹槽衬底 让纯白护甲分段在亮场景下依然立体清晰
                DrawRect(FVector2D(SegX - 1.1f * Scale, CursorY - 1.1f * Scale),
                    FVector2D(SegmentWidth + 2.2f * Scale, ArmorBarHeight + 2.2f * Scale),
                    FLinearColor(0.0f, 0.0f, 0.0f, 0.56f * ShadowAlphaScale * ArmorDim), 0);

                // 空槽霜白底轨
                DrawRect(FVector2D(SegX, CursorY), FVector2D(SegmentWidth, ArmorBarHeight),
                    AlphaTint(PrimaryTint, 0.18f * ArmorDim), 1);

                // 护甲受击延迟白色残影
                const float ArmorTrailFill = FMath::Clamp((Snapshot.ArmorDamageTrailFraction - SegStart) / (SegEnd - SegStart), 0.0f, 1.0f);
                if (ArmorTrailFill > 0.001f)
                {
                    DrawRect(FVector2D(SegX, CursorY), FVector2D(SegmentWidth * ArmorTrailFill, ArmorBarHeight),
                        AlphaTint(PrimaryTint, 0.48f), 2);
                }

                // 当前护甲纯白实心填充
                const float ArmorFill = FMath::Clamp((Snapshot.ArmorFraction - SegStart) / (SegEnd - SegStart), 0.0f, 1.0f);
                if (ArmorFill > 0.001f)
                {
                    DrawRect(FVector2D(SegX, CursorY), FVector2D(SegmentWidth * ArmorFill, ArmorBarHeight),
                        AlphaTint(PrimaryTint, 0.98f), 3);
                }
            }

            CursorY += ArmorBarHeight + HealthGap;
        }

        // 紧贴护甲下方的连续实心血量条 (形态上与上方分段护甲一眼区分 倒地态切换为战术红)
        const FLinearColor HealthFillBase = Snapshot.bDowned
            ? DownedHealthTint
            : (Snapshot.bLowHealth
                ? FMath::Lerp(FLinearColor(0.98f, 0.42f, 0.38f, 0.96f), DownedHealthTint, Snapshot.LowHealthPulse)
                : FLinearColor(0.94f, 0.96f, 0.98f, 0.94f));
        const float HealthPulseAlpha = Snapshot.bDowned
            ? FMath::Lerp(0.76f, 1.0f, Snapshot.LowHealthPulse)
            : (Snapshot.bLowHealth ? FMath::Lerp(0.72f, 1.0f, Snapshot.LowHealthPulse) : 1.0f);
        const FLinearColor HealthTrackTint = Snapshot.bDowned
            ? FLinearColor(0.42f, 0.08f, 0.08f, 0.50f)
            : AlphaTint(PrimaryTint, 0.18f);
        const FLinearColor HealthTrailTint = Snapshot.bDowned
            ? FLinearColor(1.0f, 0.45f, 0.42f, 0.62f)
            : AlphaTint(PrimaryTint, 0.50f);

        DrawRect(FVector2D(ContentLeft - 1.0f * Scale, CursorY - 1.0f * Scale),
            FVector2D(ContentWidth + 2.0f * Scale, HealthBarHeight + 2.0f * Scale),
            FLinearColor(0.0f, 0.0f, 0.0f, 0.60f * ShadowAlphaScale), 0);
        DrawRect(FVector2D(ContentLeft, CursorY), FVector2D(ContentWidth, HealthBarHeight), HealthTrackTint, 1);

        const float HealthTrailW = ContentWidth * FMath::Clamp(Snapshot.DamageTrailFraction, 0.0f, 1.0f);
        if (HealthTrailW > 0.5f)
        {
            DrawRect(FVector2D(ContentLeft, CursorY), FVector2D(HealthTrailW, HealthBarHeight), HealthTrailTint, 2);
        }

        const float HealthFillW = ContentWidth * FMath::Clamp(Snapshot.HealthFraction, 0.0f, 1.0f);
        if (HealthFillW > 0.5f)
        {
            DrawRect(FVector2D(ContentLeft, CursorY), FVector2D(HealthFillW, HealthBarHeight),
                AlphaTint(HealthFillBase, HealthPulseAlpha), 3);
        }

        // 低血量或倒地左端警示菱形
        if (Snapshot.bLowHealth || Snapshot.bDowned)
        {
            const float DiamondR = 3.6f * Scale;
            const FVector2D DiamondC(1.1f * Scale, CursorY + HealthBarHeight * 0.5f);
            DrawLine({
                DiamondC + FVector2D(0.0, -DiamondR),
                DiamondC + FVector2D(DiamondR, 0.0),
                DiamondC + FVector2D(0.0, DiamondR),
                DiamondC + FVector2D(-DiamondR, 0.0),
                DiamondC + FVector2D(0.0, -DiamondR)},
                1.4f * Scale, AlphaTint(DownedHealthTint, HealthPulseAlpha), 4, true);
        }

        // 与血量拉开明显间距的按需体力细轨 满体力时完全隐身零干扰
        const float StaminaY = CursorY + HealthBarHeight + StaminaGap;
        const float StaminaVis = FMath::Clamp(Snapshot.StaminaOpacity, 0.0f, 1.0f);
        if (StaminaVis > 0.005f)
        {
            const float StaminaFillW = ContentWidth * FMath::Clamp(Snapshot.StaminaFraction, 0.0f, 1.0f);
            DrawRect(FVector2D(ContentLeft - 0.7f * Scale, StaminaY - 0.7f * Scale),
                FVector2D(ContentWidth + 1.4f * Scale, StaminaHeight + 1.4f * Scale),
                FLinearColor(0.0f, 0.0f, 0.0f, 0.42f * StaminaVis * ShadowAlphaScale), 0);
            DrawRect(FVector2D(ContentLeft, StaminaY), FVector2D(ContentWidth, StaminaHeight),
                AlphaTint(SecondaryTint, 0.22f * StaminaVis), 1);
            DrawRect(FVector2D(ContentLeft, StaminaY), FVector2D(StaminaFillW, StaminaHeight),
                AlphaTint(SecondaryTint, 0.92f * StaminaVis), 2);
        }

        CursorY = VitalsStartY + VitalsHeight + LoadoutGap;

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
            DrawRect(FVector2D(SlotX, CursorY + LoadoutHeight - BottomBarH), FVector2D(SlotWidth, BottomBarH),
                AlphaTint(PrimaryTint, BottomBarAlpha), 1);

            // 左侧中线对齐的极小槽号 1 / 2 / 3
            const FString KeyDigit = FString::FromInt(SlotIndex + 1);
            DrawTextCrisp(KeyDigit, FVector2D(SlotX + 1.0f * Scale, CursorY + 6.0f * Scale),
                SlotNumFont, AlphaTint(PrimaryTint, Slot.bActive ? 0.98f : (Slot.bOccupied ? 0.68f : 0.30f)), 3);

            // 饱满迷你武器剪影
            if (Slot.bOccupied && WeaponSlotBrushes[SlotIndex])
            {
                DrawSilhouette(WeaponSlotBrushes[SlotIndex],
                    FVector2D(SlotX + 11.5f * Scale, CursorY + 2.0f * Scale),
                    FVector2D(SlotWidth - 12.5f * Scale, LoadoutHeight - 6.0f * Scale),
                    SlotAlpha, true, 3);
            }
        }

        // 右侧投掷物与备用消耗品区
        if (bHasThrowables)
        {
            const float DividerX = ContentLeft + WeaponZoneWidth + 6.0f * Scale;
            DrawRect(FVector2D(DividerX, CursorY + 3.0f * Scale), FVector2D(1.0f * Scale, LoadoutHeight - 6.0f * Scale),
                AlphaTint(PrimaryTint, 0.36f), 2);

            float ThrowX = DividerX + 7.0f * Scale;
            const int32 ThrowNumSize = FMath::Max(10, FMath::RoundToInt(14.0f * Scale));
            const int32 SpareNumSize = FMath::Max(8, FMath::RoundToInt(10.0f * Scale));
            const FSlateFontInfo ThrowNumFont(BoldComposite, ThrowNumSize);
            const FSlateFontInfo SpareNumFont(LightComposite, SpareNumSize);

            if (Snapshot.SelectedThrowable.bOccupied)
            {
                const float SelectedW = 40.0f * Scale;
                DrawRect(FVector2D(ThrowX - 0.5f * Scale, CursorY + LoadoutHeight - 2.5f * Scale),
                    FVector2D(SelectedW + 1.0f * Scale, 3.0f * Scale),
                    FLinearColor(0.0f, 0.0f, 0.0f, 0.42f * ShadowAlphaScale), 0);
                DrawRect(FVector2D(ThrowX, CursorY + LoadoutHeight - 2.0f * Scale), FVector2D(SelectedW, 2.0f * Scale),
                    AlphaTint(PrimaryTint, 0.92f), 1);

                if (SelectedThrowableBrush)
                {
                    DrawSilhouette(SelectedThrowableBrush,
                        FVector2D(ThrowX + 1.0f * Scale, CursorY + 1.0f * Scale),
                        FVector2D(20.0f * Scale, LoadoutHeight - 6.0f * Scale),
                        0.96f, false, 3);
                }

                if (!SelectedThrowableText.IsEmpty())
                {
                    const FVector2D NumExtent = FontMeasure->Measure(SelectedThrowableText, ThrowNumFont);
                    DrawTextCrisp(SelectedThrowableText,
                        FVector2D(ThrowX + 24.0f * Scale, CursorY + (LoadoutHeight - NumExtent.Y) * 0.42f),
                        ThrowNumFont, AlphaTint(PrimaryTint, 0.98f), 4);
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
                        0.54f, false, 3);
                }

                if (!SpareThrowableTexts[SpareIdx].IsEmpty())
                {
                    const FVector2D TinyExtent = FontMeasure->Measure(SpareThrowableTexts[SpareIdx], SpareNumFont);
                    DrawTextCrisp(SpareThrowableTexts[SpareIdx],
                        FVector2D(CellX + 14.0f * Scale, CursorY + LoadoutHeight - TinyExtent.Y - 3.0f * Scale),
                        SpareNumFont, AlphaTint(SecondaryTint, 0.84f), 4);
                }
            }
        }

        return LocalBaseLayer + 6;
    }

    FBBBDivisionHudSnapshot Snapshot;
    FLinearColor PrimaryTint = FLinearColor(1.0f, 1.0f, 1.0f, 0.98f);
    FLinearColor SecondaryTint = FLinearColor(0.92f, 0.95f, 0.99f, 0.84f);
    FLinearColor DownedHealthTint = FLinearColor(0.92f, 0.20f, 0.20f, 0.98f);
    float ContrastShadowStrength = 0.78f;
    float HudScale = 1.3f;
    FVector2D AnchorPixelOffset = FVector2D(27.0, 26.0);
    int32 ArmorSegments = 5;

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
    SlateWidget->DownedHealthTint = DownedHealthTint;
    SlateWidget->ContrastShadowStrength = ContrastShadowStrength;
    SlateWidget->ArmorSegments = ArmorSegments;
    SlateWidget->HudScale = HudScale;
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
        || LastSnapshot.bHasArmorPool != InSnapshot.bHasArmorPool
        || LastSnapshot.bHasAmmo != InSnapshot.bHasAmmo
        || LastSnapshot.bReloading != InSnapshot.bReloading
        || LastSnapshot.bLowHealth != InSnapshot.bLowHealth
        || LastSnapshot.bDowned != InSnapshot.bDowned
        || LastSnapshot.bHasActiveWeapon != InSnapshot.bHasActiveWeapon
        || LastSnapshot.LoadedAmmo != InSnapshot.LoadedAmmo
        || LastSnapshot.CapacityAmmo != InSnapshot.CapacityAmmo
        || LastSnapshot.ActiveWeaponIcon != InSnapshot.ActiveWeaponIcon
        || LastSnapshot.SelectedThrowable != InSnapshot.SelectedThrowable
        || LastSnapshot.TeammateOverheads.Num() != InSnapshot.TeammateOverheads.Num()
        || !FMath::IsNearlyEqual(LastSnapshot.ArmorFraction, InSnapshot.ArmorFraction, 0.001f)
        || !FMath::IsNearlyEqual(LastSnapshot.ArmorDamageTrailFraction, InSnapshot.ArmorDamageTrailFraction, 0.001f)
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
        for (int32 Index = 0; Index < InSnapshot.TeammateOverheads.Num(); ++Index)
        {
            if (!LastSnapshot.TeammateOverheads[Index].NearlyEquals(InSnapshot.TeammateOverheads[Index]))
            {
                bChanged = true;
                break;
            }
        }
    }

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
