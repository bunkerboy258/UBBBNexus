#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

float ABBBPlayerController::GetHudHealthFraction() const
{
    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    if (!PawnCharacter || PawnCharacter->GetLifePhase() == EBBBCharacterLifePhase::Dead)
    {
        return 0.0f;
    }
    const auto &Config = PawnCharacter->GetCharacterConfig();
    const float Maximum = PawnCharacter->GetLifePhase() == EBBBCharacterLifePhase::Downed
        ? Config.DownedHealth : Config.MaximumHealth;
    return Maximum > 0.0f ? FMath::Clamp(PawnCharacter->GetHealth() / Maximum, 0.0f, 1.0f) : 0.0f;
}

bool ABBBPlayerController::ShouldShowAimHud() const
{
    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    return PawnCharacter && !IsPlayerMenuOpen() && !bShowMouseCursor
        && PawnCharacter->RuntimeData.Animation.ReadAnimationFactState().bIsAiming
        && IsValid(PawnCharacter->GetHeldEquipment());
}

float ABBBPlayerController::GetHudStaminaFraction() const
{
    const auto *PawnCharacter = GetItemCharacter();
    if (!PawnCharacter || PawnCharacter->GetLifePhase() != EBBBCharacterLifePhase::Alive)
    {
        return 0.0f;
    }
    const float Maximum = PawnCharacter->GetMaximumStamina();
    return Maximum > 0.0f ? FMath::Clamp(PawnCharacter->GetStamina() / Maximum, 0.0f, 1.0f) : 0.0f;
}

UTexture2D *ABBBPlayerController::GetHudWeaponIcon() const
{
    const auto *Equipment = Cast<ABBBEquipment>(GetActiveItem());
    const auto *Definition = IsValid(Equipment) ? Equipment->GetDefinition() : nullptr;
    return Definition ? Definition->Icon.Get() : nullptr;
}

bool ABBBPlayerController::ReadHudAmmo(int32 &Loaded, int32 &Capacity, bool &bContinuous, bool &bReloading) const
{
    Loaded = 0;
    Capacity = 0;
    bContinuous = false;
    bReloading = false;
    const auto *Equipment = Cast<ABBBEquipment>(GetActiveItem());
    if (!IsValid(Equipment) || !Equipment->ReadAmmoDisplay(Loaded, Capacity, bContinuous) || Capacity <= 0)
    {
        return false;
    }
    Loaded = FMath::Clamp(Loaded, 0, Capacity);
    bReloading = Equipment->IsReloading();
    return true;
}

TOptional<int32> ABBBPlayerController::GetHudPing() const
{
    const auto *State = GetPlayerState<APlayerState>();
    if (!State || GetNetMode() != NM_Client)
    {
        return {};
    }
    const float Ping = State->GetPingInMilliseconds();
    return FMath::IsFinite(Ping) && Ping > 0.0f ? TOptional<int32>(FMath::RoundToInt(Ping)) : TOptional<int32>();
}

bool ABBBPlayerController::GetActualAimWorldPosition(FVector &WorldPosition) const
{
    WorldPosition = FVector::ZeroVector;
    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    const ABBBEquipment *Equipment = PawnCharacter ? PawnCharacter->GetHeldEquipment() : nullptr;
    FTransform Muzzle;
    if (!GetWorld() || !IsValid(Equipment) || !Equipment->TryGetMuzzleTransform(Muzzle))
    {
        return false;
    }
    const FVector Start = Muzzle.GetLocation();
    const FVector End = Start + Muzzle.GetUnitAxis(EAxis::X) * 100000.0f;
    FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBHudAim), true, PawnCharacter);
    Query.AddIgnoredActor(Equipment);
    FHitResult Hit;
    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Query);
    WorldPosition = bHit ? Hit.ImpactPoint : End;
    if (const UBBBMassSubsystem *Mass = GetWorld()->GetSubsystem<UBBBMassSubsystem>())
    {
        FMassEntityHandle HitEntity;
        float HitTime = 1.0f;
        FVector HitPosition;
        FVector HitNormal;
        EPhysicalSurface HitSurface;
        uint8 HitPart = 0;
        // 场景遮挡裁短射线 后方实体不覆盖枪口最近落点
        if (Mass->TraceEntities(Start, WorldPosition, 0.0f, {}, HitEntity, HitTime,
            HitPosition, HitNormal, HitSurface, HitPart))
        {
            WorldPosition = HitPosition;
        }
    }
    return true;
}

bool ABBBPlayerController::GetActualAimScreenPosition(FVector2D &ScreenPosition) const
{
    ScreenPosition = FVector2D::ZeroVector;
    FVector WorldPosition;
    return GetActualAimWorldPosition(WorldPosition)
        && ProjectWorldLocationToScreen(WorldPosition, ScreenPosition, true);
}

bool ABBBPlayerController::ReadHudHealth(float &Health, float &Maximum, uint8 &Phase) const
{
    Health = 0.0f;
    Maximum = 0.0f;
    Phase = 255;
    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    if (!PawnCharacter)
    {
        return false;
    }
    Phase = static_cast<uint8>(PawnCharacter->GetLifePhase());
    Health = PawnCharacter->GetHealth();
    Maximum = PawnCharacter->GetLifePhase() == EBBBCharacterLifePhase::Downed
        ? PawnCharacter->GetCharacterConfig().DownedHealth : PawnCharacter->GetCharacterConfig().MaximumHealth;
    return true;
}

bool ABBBPlayerController::ShouldShowCrosshair() const
{
    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    return PawnCharacter && PawnCharacter->GetLifePhase() == EBBBCharacterLifePhase::Alive
        && !IsPlayerMenuOpen() && !IsBackpackOpen() && !bShowMouseCursor;
}
