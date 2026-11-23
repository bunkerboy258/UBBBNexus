#include "BBBWork/UBBBNexus/Player/UI/BBBCombatHudWidget.h"

#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBCombatHudIndicator.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
void SetShown(UWidget *Widget, bool bShown)
{
    if (Widget)
    {
        const ESlateVisibility Visibility = bShown ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
        if (Widget->GetVisibility() != Visibility)
        {
            Widget->SetVisibility(Visibility);
        }
    }
}
}

UBBBCombatHudWidget::UBBBCombatHudWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(
        TEXT("/Game/_Project/Characters/Items/Shared/M_HudItemSilhouette.M_HudItemSilhouette"));
    ItemIconMaterial = Material.Object;
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
        return;
    }

    const double Now = FPlatformTime::Seconds();
    UpdateVitals(*Controller, DeltaTime, Now);
    if (bItemsDirty)
    {
        UpdateItems(*Controller);
        bItemsDirty = false;
    }

    if (SampleStartedAt == 0.0)
    {
        SetShown(GetWidgetFromName(TEXT("Telemetry")), false);
        SampleStartedAt = Now;
    }
    ++SampleFrames;
    const double SampleDuration = Now - SampleStartedAt;
    if (SampleDuration >= 0.5)
    {
        SetCount(Cast<UTextBlock>(GetWidgetFromName(TEXT("FrameRateValue"))), FMath::RoundToInt(SampleFrames / SampleDuration));
        SetShown(GetWidgetFromName(TEXT("Telemetry")), true);
        const TOptional<int32> Ping = Controller->GetHudPing();
        SetShown(GetWidgetFromName(TEXT("PingGroup")), Ping.IsSet());
        if (Ping.IsSet())
        {
            SetCount(Cast<UTextBlock>(GetWidgetFromName(TEXT("PingValue"))), Ping.GetValue());
        }
        SampleStartedAt = Now;
        SampleFrames = 0;
    }

    SetShown(Crosshair, Controller->ShouldShowCrosshair());
    const bool bAiming = Controller->ShouldShowAimHud();
    if (AimPoint)
    {
        FVector2D Projected;
        int32 Width = 0;
        int32 Height = 0;
        Controller->GetViewportSize(Width, Height);
        if (const ULocalPlayer *LocalPlayer = Controller->GetLocalPlayer())
        {
            Width = FMath::RoundToInt(Width * LocalPlayer->Size.X);
            Height = FMath::RoundToInt(Height * LocalPlayer->Size.Y);
        }
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

    int32 Loaded = 0;
    int32 Capacity = 0;
    bool bContinuous = false;
    bool bReloading = false;
    const bool bHasAmmo = Controller->ReadHudAmmo(Loaded, Capacity, bContinuous, bReloading);
    SetShown(GetWidgetFromName(TEXT("AmmoReadout")), bHasAmmo);
    SetShown(GetWidgetFromName(TEXT("AmmoGroup")), bHasAmmo);
    SetShown(AmmoRail, bAiming && bHasAmmo);
    SetShown(GetWidgetFromName(TEXT("ReloadBounds")), bHasAmmo && bReloading);
    if (bHasAmmo)
    {
        auto *LoadedLabel = Cast<UTextBlock>(GetWidgetFromName(TEXT("LoadedAmmo")));
        SetCount(LoadedLabel, Loaded);
        if (LoadedLabel)
        {
            LoadedLabel->SetRenderOpacity(Loaded > 0 ? 1.0f : 0.65f);
        }
        SetCount(Cast<UTextBlock>(GetWidgetFromName(TEXT("CapacityAmmo"))), Capacity, false, true);
        if (AmmoRail)
        {
            AmmoRail->SetAmmo(Loaded, Capacity, bContinuous);
        }
        if (Reload && bReloading)
        {
            Reload->SetReloadAngle(FMath::Fmod(Now * 360.0, 360.0));
        }
    }
}

void UBBBCombatHudWidget::UpdateVitals(const ABBBPlayerController &Controller, float DeltaTime, double Now)
{
    const float Fraction = Controller.GetHudHealthFraction();
    if (StaminaBar)
    {
        StaminaBar->SetPercent(Controller.GetHudStaminaFraction());
    }

    DamageAge += DeltaTime;
    float Health = 0.0f;
    float Maximum = 0.0f;
    uint8 Phase = MAX_uint8;
    if (!Controller.ReadHudHealth(Health, Maximum, Phase))
    {
        ObservedPawn.Reset();
        bLowHealth = false;
        DamageAge = 1.0f;
    }
    else
    {
        const bool bReset = ObservedPawn.Get() != Controller.GetPawn()
            || PreviousPhase != Phase || PreviousMaximum != Maximum;
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
        ObservedPawn = Controller.GetPawn();
        PreviousHealth = Health;
        PreviousMaximum = Maximum;
        PreviousPhase = Phase;
    }

    if (DamageTrailBar)
    {
        DamageTrailBar->SetPercent(FMath::Lerp(DamageTrail, Fraction, FMath::Clamp((DamageAge - 0.15f) / 0.45f, 0.0f, 1.0f)));
    }
    if (HealthBar)
    {
        HealthBar->SetPercent(Fraction);
        const float Opacity = bLowHealth ? 0.68f + 0.28f * FMath::Abs(FMath::Sin(Now * 4.0)) : 1.0f;
        HealthBar->SetRenderOpacity(Opacity);
    }
}

void UBBBCombatHudWidget::UpdateItems(const ABBBPlayerController &Controller)
{
    int32 Start = 0;
    int32 Count = 0;
    Controller.GetConsumableQuickRange(Start, Count);
    const int32 Selected = Controller.GetSelectedThrowableSlot();
    const bool bHasSelection = Selected >= 0 && Selected < Count;
    SetShown(GetWidgetFromName(TEXT("SelectedItem")), bHasSelection);
    if (bHasSelection)
    {
        const auto Item = Controller.GetItemDisplayData(Start + Selected);
        SetIcon(Cast<UImage>(GetWidgetFromName(TEXT("SelectedItemIcon"))), Item.Icon);
        SetCount(Cast<UTextBlock>(GetWidgetFromName(TEXT("SelectedItemCount"))), Item.Quantity);
    }
    int32 Row = 1;
    for (int32 Index = 0; Index < Count; ++Index)
    {
        const auto Item = Controller.GetItemDisplayData(Start + Index);
        if (Item.bOccupied && Index != Selected)
        {
            SetItemRow(TEXT("SpareItem"), Row++, Item.Icon, Item.Quantity, true);
        }
    }
    for (; Row <= Count; ++Row)
    {
        SetItemRow(TEXT("SpareItem"), Row, nullptr, INDEX_NONE);
    }

    UTexture2D *WeaponIcon = Controller.GetHudWeaponIcon();
    SetShown(GetWidgetFromName(TEXT("CurrentWeapon")), WeaponIcon != nullptr);
    SetIcon(Cast<UImage>(GetWidgetFromName(TEXT("CurrentWeaponIcon"))), WeaponIcon);
    Row = 1;
    Count = Controller.GetQuickAccessSlotCount();
    for (int32 Index = 0; Index < Count; ++Index)
    {
        const auto Item = Controller.GetItemDisplayData(Index);
        if (Item.bOccupied && !Item.bActive)
        {
            SetItemRow(TEXT("SpareWeapon"), Row++, Item.Icon, Item.QuickIndex + 1);
        }
    }
    for (; Row <= Count; ++Row)
    {
        SetItemRow(TEXT("SpareWeapon"), Row, nullptr, INDEX_NONE);
    }
}

void UBBBCombatHudWidget::SetIcon(UImage *Image, UTexture2D *Texture)
{
    if (!Image)
    {
        return;
    }
    SetShown(Image, Texture != nullptr);
    if (IconTextures.FindRef(Image) == Texture)
    {
        return;
    }
    if (!Texture)
    {
        IconTextures.Remove(Image);
        return;
    }
    IconTextures.Add(Image, Texture);
    if (ItemIconMaterial)
    {
        if (!Image->GetDynamicMaterial())
        {
            Image->SetBrushFromMaterial(ItemIconMaterial);
        }
        if (UMaterialInstanceDynamic *Material = Image->GetDynamicMaterial())
        {
            Material->SetTextureParameterValue(TEXT("ImageTexture"), Texture);
            FVector2D DisplaySize(Texture->GetSizeX(), Texture->GetSizeY());
            const FBox2f UVRegion = Image->GetBrush().GetUVRegion();
            if (UVRegion.bIsValid)
            {
                DisplaySize *= FVector2D(UVRegion.GetSize());
            }
            Image->SetDesiredSizeOverride(DisplaySize);
        }
    }
}

void UBBBCombatHudWidget::SetCount(UTextBlock *Label, int32 Number, bool bPad, bool bDenominator)
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
    const FString Text = bDenominator ? FString::Printf(TEXT("/%d"), Number)
        : (bPad ? FString::Printf(TEXT("%02d"), Number) : FString::FromInt(Number));
    Label->SetText(FText::AsCultureInvariant(Text));
}

void UBBBCombatHudWidget::SetItemRow(const TCHAR *Prefix, int32 Row, UTexture2D *Texture, int32 Number, bool bPad)
{
    const FString Name = FString::Printf(TEXT("%s%d"), Prefix, Row);
    SetShown(GetWidgetFromName(FName(*Name)), Number != INDEX_NONE);
    if (Number != INDEX_NONE)
    {
        SetIcon(Cast<UImage>(GetWidgetFromName(FName(*(Name + TEXT("Icon"))))), Texture);
        SetCount(Cast<UTextBlock>(GetWidgetFromName(FName(*(Name + TEXT("Count"))))), Number, bPad);
    }
}
