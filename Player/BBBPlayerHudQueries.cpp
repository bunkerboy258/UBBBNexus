#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Engine/World.h"

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
        && IsValid(PawnCharacter->GetActiveEquipment());
}

bool ABBBPlayerController::GetActualAimScreenPosition(FVector2D &ScreenPosition) const
{
    ScreenPosition = FVector2D::ZeroVector;
    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    const ABBBEquipment *Equipment = PawnCharacter ? PawnCharacter->GetActiveEquipment() : nullptr;
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
    return ProjectWorldLocationToScreen(bHit ? Hit.ImpactPoint : End, ScreenPosition, true);
}
