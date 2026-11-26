#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
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

float ABBBPlayerController::GetHudArmorFraction() const
{
    float Armor = 0.0f;
    float Maximum = 0.0f;
    if (!ReadHudArmor(Armor, Maximum) || Maximum <= 0.0f)
    {
        return 0.0f;
    }
    return FMath::Clamp(Armor / Maximum, 0.0f, 1.0f);
}

bool ABBBPlayerController::ReadHudArmor(float &Armor, float &Maximum) const
{
    Armor = 0.0f;
    Maximum = 0.0f;
    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    if (!PawnCharacter)
    {
        return false;
    }
    Maximum = FMath::Max(0.0f, PawnCharacter->GetCharacterConfig().MaximumArmor);
    Armor = PawnCharacter->GetLifePhase() == EBBBCharacterLifePhase::Alive
        ? FMath::Clamp(PawnCharacter->GetArmor(), 0.0f, Maximum)
        : 0.0f;
    return true;
}

bool ABBBPlayerController::CheckHudCharacterLineOfSight(const ABBBCharacter *Target, FVector &OutOverheadWorldPosition) const
{
    OutOverheadWorldPosition = FVector::ZeroVector;
    if (!IsValid(Target) || !GetWorld())
    {
        return false;
    }

    FVector HeadLocation = Target->GetActorLocation();
    float FallbackTopZ = HeadLocation.Z + 88.0f;
    if (const UCapsuleComponent *Capsule = Target->GetCapsuleComponent())
    {
        const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
        HeadLocation.Z = Target->GetActorLocation().Z + HalfHeight * 0.78f;
        FallbackTopZ = Target->GetActorLocation().Z + HalfHeight + 20.0f;
    }

    bool bHasHeadBone = false;
    if (const USkeletalMeshComponent *Mesh = Target->GetMesh())
    {
        static const FName HeadBoneName(TEXT("head"));
        if (Mesh->GetBoneIndex(HeadBoneName) != INDEX_NONE)
        {
            HeadLocation = Mesh->GetBoneLocation(HeadBoneName);
            bHasHeadBone = true;
        }
    }

    OutOverheadWorldPosition = bHasHeadBone
        ? (HeadLocation + FVector(0.0, 0.0, 28.0))
        : FVector(HeadLocation.X, HeadLocation.Y, FallbackTopZ);

    FVector ViewLocation = FVector::ZeroVector;
    FRotator ViewRotation = FRotator::ZeroRotator;
    GetPlayerViewPoint(ViewLocation, ViewRotation);

    FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBHudTeammateLos), false, GetPawn());
    Query.AddIgnoredActor(Target);
    if (const ABBBCharacter *LocalCharacter = GetItemCharacter())
    {
        Query.AddIgnoredActor(LocalCharacter);
        if (const AActor *LocalHeld = LocalCharacter->GetHeldItem())
        {
            Query.AddIgnoredActor(LocalHeld);
        }
    }
    if (const AActor *TargetHeld = Target->GetHeldItem())
    {
        Query.AddIgnoredActor(TargetHeld);
    }

    FHitResult Hit;
    return !GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, HeadLocation, ECC_Visibility, Query);
}

bool ABBBPlayerController::ProjectHudWorldPoint(const FVector &WorldPosition, FVector2D &OutScreenPosition, float &OutDistance) const
{
    OutScreenPosition = FVector2D::ZeroVector;
    OutDistance = 0.0f;

    FVector ViewLocation = FVector::ZeroVector;
    FRotator ViewRotation = FRotator::ZeroRotator;
    GetPlayerViewPoint(ViewLocation, ViewRotation);

    const FVector CamForward = FRotationMatrix(ViewRotation).GetUnitAxis(EAxis::X);
    if (FVector::DotProduct(WorldPosition - ViewLocation, CamForward) <= 10.0)
    {
        return false;
    }

    OutDistance = static_cast<float>(FVector::Dist(ViewLocation, WorldPosition));
    return ProjectWorldLocationToScreen(WorldPosition, OutScreenPosition, true);
}

bool ABBBPlayerController::ShouldShowCrosshair() const
{
    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    return PawnCharacter && PawnCharacter->GetLifePhase() == EBBBCharacterLifePhase::Alive
        && !IsPlayerMenuOpen() && !IsBackpackOpen() && !bShowMouseCursor;
}

bool ABBBPlayerController::GetHudBackpackAnchorScreenPosition(FVector2D &ScreenPosition, float &DistanceScale) const
{
    ScreenPosition = FVector2D::ZeroVector;
    DistanceScale = 1.0f;

    const ABBBCharacter *PawnCharacter = GetItemCharacter();
    if (!PawnCharacter)
    {
        return false;
    }

    FVector BaseLocation = PawnCharacter->GetActorLocation();
    if (const UCapsuleComponent *Capsule = PawnCharacter->GetCapsuleComponent())
    {
        BaseLocation.Z += Capsule->GetScaledCapsuleHalfHeight() * 0.42f;
    }

    if (const USkeletalMeshComponent *Mesh = PawnCharacter->GetMesh())
    {
        static const FName SpineMidName(TEXT("spine_02"));
        static const FName SpineHighName(TEXT("spine_03"));
        if (Mesh->GetBoneIndex(SpineMidName) != INDEX_NONE)
        {
            BaseLocation = Mesh->GetBoneLocation(SpineMidName);
        }
        else if (Mesh->GetBoneIndex(SpineHighName) != INDEX_NONE)
        {
            BaseLocation = Mesh->GetBoneLocation(SpineHighName);
        }
    }

    FVector ViewLocation = FVector::ZeroVector;
    FRotator ViewRotation = FRotator::ZeroRotator;
    GetPlayerViewPoint(ViewLocation, ViewRotation);

    const FRotationMatrix ViewMatrix(ViewRotation);
    const FVector CamForward = ViewMatrix.GetUnitAxis(EAxis::X);
    const FVector CamRight = ViewMatrix.GetUnitAxis(EAxis::Y);
    const FVector CamUp = ViewMatrix.GetUnitAxis(EAxis::Z);

    // 锚定在角色中背背包右侧略偏下位置 保留 1.5 倍横向呼吸空隙并自然避开中央准星视线
    const FVector WorldAnchor = BaseLocation + CamRight * 49.5 - CamUp * 4.0 - CamForward * 4.0;
    if (FVector::DotProduct(WorldAnchor - ViewLocation, CamForward) <= 10.0)
    {
        return false;
    }

    if (!ProjectWorldLocationToScreen(WorldAnchor, ScreenPosition, true))
    {
        return false;
    }

    const double Distance = FVector::Dist(ViewLocation, WorldAnchor);
    DistanceScale = FMath::Clamp(static_cast<float>(FMath::Pow(375.0 / FMath::Max(140.0, Distance), 0.22)), 0.88f, 1.06f);
    return true;
}
