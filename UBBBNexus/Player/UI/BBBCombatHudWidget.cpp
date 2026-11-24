#include "BBBWork/UBBBNexus/Player/UI/BBBCombatHudWidget.h"

#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBCombatHudIndicator.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"

namespace
{
void SetShown(UWidget *Widget, bool bShown)
{
    if (!Widget)
    {
        return;
    }

    const ESlateVisibility Visibility = bShown ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
    if (Widget->GetVisibility() != Visibility)
    {
        Widget->SetVisibility(Visibility);
    }
}

void ReadLocalPlayerViewportSize(const ABBBPlayerController &Controller, int32 &OutWidth, int32 &OutHeight)
{
    OutWidth = 0;
    OutHeight = 0;
    Controller.GetViewportSize(OutWidth, OutHeight);
    if (const ULocalPlayer *LocalPlayer = Controller.GetLocalPlayer())
    {
        OutWidth = FMath::RoundToInt(OutWidth * LocalPlayer->Size.X);
        OutHeight = FMath::RoundToInt(OutHeight * LocalPlayer->Size.Y);
    }
}
}

UBBBCombatHudWidget::UBBBCombatHudWidget(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UBBBCombatHudWidget::UpdatePresentation(const ABBBPlayerController *Controller, const FGeometry &Geometry, float DeltaTime)
{
    const bool bShown = Controller && Controller->HasItemInventory()
        && !Controller->IsPlayerMenuOpen() && !Controller->IsBackpackOpen();
    SetShown(WidgetTree ? WidgetTree->RootWidget.Get() : nullptr, bShown);
    if (!bShown)
    {
        SampleStartedAt = 0.0;
        SampleFrames = 0;
        bAnchorInitialized = false;
        return;
    }

    const double Now = FPlatformTime::Seconds();
    UpdateVitals(*Controller, DeltaTime, Now);
    if (bItemsDirty)
    {
        UpdateItems(*Controller);
        bItemsDirty = false;
    }

    // 边缘极静遥测 每半秒刷新一次纯数字 无文字标签
    if (SampleStartedAt == 0.0)
    {
        SetShown(Telemetry, false);
        SampleStartedAt = Now;
    }

    ++SampleFrames;
    const double SampleDuration = Now - SampleStartedAt;
    if (SampleDuration >= 0.5)
    {
        SetCount(FrameRateValue, FMath::RoundToInt(SampleFrames / SampleDuration));
        SetShown(Telemetry, true);
        const TOptional<int32> Ping = Controller->GetHudPing();
        SetShown(PingGroup, Ping.IsSet());
        if (Ping.IsSet())
        {
            SetCount(PingValue, Ping.GetValue());
        }

        SampleStartedAt = Now;
        SampleFrames = 0;
    }

    // 中央准星与枪口实弹落点
    SetShown(Crosshair, Controller->ShouldShowCrosshair());
    const bool bAiming = Controller->ShouldShowAimHud();
    const float AimStep = AimFadeSeconds > 0.0f ? DeltaTime / AimFadeSeconds : 1.0f;
    AimBlend = FMath::FInterpConstantTo(AimBlend, bAiming ? 1.0f : 0.0f, 1.0f, AimStep);
    CachedSnapshot.AimBlend = AimBlend;

    if (AimPoint)
    {
        FVector2D Projected = FVector2D::ZeroVector;
        int32 Width = 0;
        int32 Height = 0;
        ReadLocalPlayerViewportSize(*Controller, Width, Height);
        const bool bProjected = bAiming && Width > 0 && Height > 0
            && Controller->GetActualAimScreenPosition(Projected)
            && Projected.X >= 0.0 && Projected.Y >= 0.0 && Projected.X <= Width && Projected.Y <= Height;
        if (bProjected)
        {
            const FVector2D Normalized(Projected.X / Width, Projected.Y / Height);
            AimPoint->SetRenderTranslation((Normalized - FVector2D(0.5)) * Geometry.GetLocalSize());
        }

        SetShown(AimPoint, bProjected);
    }

    // 弹药与换弹状态 同步给准星短弧与背包悬浮主集群
    int32 Loaded = 0;
    int32 Capacity = 0;
    bool bContinuous = false;
    bool bReloading = false;
    const bool bHasAmmo = Controller->ReadHudAmmo(Loaded, Capacity, bContinuous, bReloading);
    SetShown(AmmoRail, bAiming && bHasAmmo);

    CachedSnapshot.bHasAmmo = bHasAmmo;
    CachedSnapshot.LoadedAmmo = Loaded;
    CachedSnapshot.CapacityAmmo = Capacity;
    CachedSnapshot.bAmmoContinuous = bContinuous;
    CachedSnapshot.bReloading = bHasAmmo && bReloading;
    if (bHasAmmo && bReloading)
    {
        CachedSnapshot.ReloadAngle = static_cast<float>(FMath::Fmod(Now * 360.0, 360.0));
    }

    if (bHasAmmo && AmmoRail)
    {
        AmmoRail->SetAmmo(Loaded, Capacity, bContinuous);
    }

    UpdateAnchorMotion(*Controller, Geometry, DeltaTime);

    if (BackpackHud)
    {
        BackpackHud->SetSnapshot(CachedSnapshot);
    }
}

void UBBBCombatHudWidget::UpdateVitals(const ABBBPlayerController &Controller, float DeltaTime, double Now)
{
    const float HealthFraction = Controller.GetHudHealthFraction();
    const float StaminaFraction = Controller.GetHudStaminaFraction();

    DamageAge += DeltaTime;
    float Health = 0.0f;
    float Maximum = 0.0f;
    uint8 Phase = MAX_uint8;
    if (!Controller.ReadHudHealth(Health, Maximum, Phase))
    {
        ObservedPawn.Reset();
        bLowHealth = false;
        DamageAge = 1.0f;
        DamageTrail = HealthFraction;
    }
    else
    {
        const bool bReset = ObservedPawn.Get() != Controller.GetPawn()
            || PreviousPhase != Phase || PreviousMaximum != Maximum;
        if (bReset || Health > PreviousHealth)
        {
            DamageAge = 1.0f;
            DamageTrail = HealthFraction;
        }

        if (!bReset && Phase == 0 && Health < PreviousHealth)
        {
            const float PreviousFraction = PreviousHealth / FMath::Max(1.0f, Maximum);
            DamageTrail = FMath::Lerp(DamageTrail, PreviousFraction,
                FMath::Clamp((DamageAge - DeltaTime - 0.14f) / 0.44f, 0.0f, 1.0f));
            DamageAge = 0.0f;
        }

        bLowHealth = Phase == 0 && HealthFraction <= (bLowHealth && !bReset ? 0.35f : 0.30f);
        ObservedPawn = Controller.GetPawn();
        PreviousHealth = Health;
        PreviousMaximum = Maximum;
        PreviousPhase = Phase;
    }

    // 满体力静默淡出 消耗或恢复时即时显现
    if (StaminaFraction >= 0.995f)
    {
        StaminaFullSeconds = FMath::Min(StaminaFullSeconds + DeltaTime, 3.0f);
    }
    else
    {
        StaminaFullSeconds = 0.0f;
    }

    const float QuietProgress = FMath::Clamp((StaminaFullSeconds - 1.0f) / 0.5f, 0.0f, 1.0f);
    CachedSnapshot.HealthFraction = HealthFraction;
    CachedSnapshot.DamageTrailFraction = FMath::Lerp(DamageTrail, HealthFraction,
        FMath::Clamp((DamageAge - 0.14f) / 0.44f, 0.0f, 1.0f));
    CachedSnapshot.StaminaFraction = StaminaFraction;
    CachedSnapshot.StaminaOpacity = FMath::Lerp(0.92f, 0.26f, QuietProgress);
    CachedSnapshot.bLowHealth = bLowHealth;
    CachedSnapshot.bDowned = Phase == 1;
    CachedSnapshot.LowHealthPulse = 0.5f + 0.5f * static_cast<float>(FMath::Sin(Now * 5.2));
}

void UBBBCombatHudWidget::UpdateItems(const ABBBPlayerController &Controller)
{
    CachedSnapshot.ActiveWeaponIcon = Controller.GetHudWeaponIcon();
    CachedSnapshot.bHasActiveWeapon = CachedSnapshot.ActiveWeaponIcon != nullptr;

    const int32 WeaponCount = FMath::Min(Controller.GetQuickAccessSlotCount(), FBBBDivisionHudSnapshot::MaxWeaponSlots);
    for (int32 Index = 0; Index < FBBBDivisionHudSnapshot::MaxWeaponSlots; ++Index)
    {
        FBBBDivisionHudSlotSnapshot &SlotData = CachedSnapshot.WeaponSlots[Index];
        SlotData = FBBBDivisionHudSlotSnapshot{};
        if (Index < WeaponCount)
        {
            const FBBBPlayerItemDisplayData Item = Controller.GetItemDisplayData(Index);
            SlotData.bOccupied = Item.bOccupied;
            SlotData.bActive = Item.bActive;
            SlotData.Icon = Item.bOccupied ? Item.Icon : nullptr;
            SlotData.Number = Index + 1;
        }
    }

    int32 ConsumableStart = 0;
    int32 ConsumableCount = 0;
    Controller.GetConsumableQuickRange(ConsumableStart, ConsumableCount);
    const int32 Selected = Controller.GetSelectedThrowableSlot();

    CachedSnapshot.SelectedThrowable = FBBBDivisionHudSlotSnapshot{};
    if (Selected >= 0 && Selected < ConsumableCount)
    {
        const FBBBPlayerItemDisplayData Item = Controller.GetItemDisplayData(ConsumableStart + Selected);
        CachedSnapshot.SelectedThrowable.bOccupied = Item.bOccupied;
        CachedSnapshot.SelectedThrowable.bActive = true;
        CachedSnapshot.SelectedThrowable.Icon = Item.Icon;
        CachedSnapshot.SelectedThrowable.Number = Item.Quantity;
    }

    for (int32 SpareIdx = 0; SpareIdx < FBBBDivisionHudSnapshot::MaxSpareThrowables; ++SpareIdx)
    {
        CachedSnapshot.SpareThrowables[SpareIdx] = FBBBDivisionHudSlotSnapshot{};
    }

    int32 SpareSlot = 0;
    for (int32 Index = 0; Index < ConsumableCount && SpareSlot < FBBBDivisionHudSnapshot::MaxSpareThrowables; ++Index)
    {
        if (Index == Selected)
        {
            continue;
        }

        const FBBBPlayerItemDisplayData Item = Controller.GetItemDisplayData(ConsumableStart + Index);
        if (Item.bOccupied)
        {
            FBBBDivisionHudSlotSnapshot &Target = CachedSnapshot.SpareThrowables[SpareSlot++];
            Target.bOccupied = true;
            Target.Icon = Item.Icon;
            Target.Number = Item.Quantity;
        }
    }
}

void UBBBCombatHudWidget::UpdateAnchorMotion(const ABBBPlayerController &Controller, const FGeometry &Geometry, float DeltaTime)
{
    const FVector2D LocalSize = Geometry.GetLocalSize();
    CachedSnapshot.ViewportSize = LocalSize;

    int32 Width = 0;
    int32 Height = 0;
    ReadLocalPlayerViewportSize(Controller, Width, Height);

    FVector2D Projected = FVector2D::ZeroVector;
    float TargetDistanceScale = 1.0f;
    const bool bProjected = Width > 0 && Height > 0 && LocalSize.X > 0.0 && LocalSize.Y > 0.0
        && Controller.GetHudBackpackAnchorScreenPosition(Projected, TargetDistanceScale);

    if (!bProjected)
    {
        CachedSnapshot.bHasAnchor = bAnchorInitialized;
        return;
    }

    const FVector2D TargetLocal(Projected.X / Width * LocalSize.X, Projected.Y / Height * LocalSize.Y);
    if (!bAnchorInitialized || DeltaTime <= 0.0f || DeltaTime > 0.25f)
    {
        SmoothedAnchor = TargetLocal;
        AnchorVelocity = FVector2D::ZeroVector;
        SmoothedDistanceScale = TargetDistanceScale;
        bAnchorInitialized = true;
    }
    else
    {
        // 临界阻尼弹簧平滑 过滤跑步微抖并保留镜头转动的轻盈全息跟随感
        FMath::SpringDamperSmoothing(SmoothedAnchor, AnchorVelocity, TargetLocal, FVector2D::ZeroVector,
            DeltaTime, AnchorSmoothSeconds, 1.0f);
        SmoothedDistanceScale = FMath::FInterpTo(SmoothedDistanceScale, TargetDistanceScale, DeltaTime, 10.0f);
    }

    CachedSnapshot.bHasAnchor = true;
    CachedSnapshot.AnchorPosition = SmoothedAnchor;
    CachedSnapshot.DistanceScale = SmoothedDistanceScale;
}

void UBBBCombatHudWidget::SetCount(UTextBlock *Label, int32 Number)
{
    if (!Label)
    {
        return;
    }

    const int32 *Displayed = DisplayedNumbers.Find(Label->GetFName());
    if (Displayed && *Displayed == Number)
    {
        return;
    }

    DisplayedNumbers.Add(Label->GetFName(), Number);
    Label->SetText(FText::AsCultureInvariant(FString::FromInt(Number)));
}
