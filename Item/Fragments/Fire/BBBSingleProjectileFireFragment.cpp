
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/BBBSingleProjectileFireFragment.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Weapon/BBBWeaponActor.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Weapon/Projectile/BBBBulletActor.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/BBBFireEvents.h"

UBBBSingleProjectileFireFragment::UBBBSingleProjectileFireFragment()
{

    BulletActorClass = ABBBBulletActor::StaticClass();
}

bool UBBBSingleProjectileFireFragment::Fire(
    ABBBWeaponActor &Weapon,
    UBBBFireRuntimeData &RuntimeData,
    FBBBShotEvent &OutShotEvent) const
{

    UWorld *World = Weapon.GetWorld();
    if (!ensureMsgf(World, TEXT("[UBBBI]Projectile fire failed: world context is null")))
    {
        return false;
    }

    const float Now = World->GetTimeSeconds();
    if (Now - RuntimeData.LastFireTime < FireInterval)
    {
        return false;
    }

    UStaticMeshComponent *WeaponMesh = Weapon.GetWeaponMesh();
    if (!ensureMsgf(WeaponMesh, TEXT("[UBBBI]Projectile fire failed: weapon mesh is null")))
    {
        return false;
    }

    if (!WeaponMesh->DoesSocketExist(MuzzleSocketName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon fire requires a valid muzzle socket"));
        return false;
    }

    const FTransform MuzzleTransform = WeaponMesh->GetSocketTransform(MuzzleSocketName, RTS_World);

    const FVector ShotDirection = MuzzleTransform.GetUnitAxis(EAxis::Z);
    if (ShotDirection.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon muzzle socket has an invalid forward axis"));
        return false;
    }

    OutShotEvent.MuzzleLocation = MuzzleTransform.GetLocation();
    OutShotEvent.ShotDirection = ShotDirection;

    if (!Weapon.SpawnBullet(BulletActorClass, MuzzleSpeed, OutShotEvent))
    {
        return false;
    }

    RuntimeData.LastFireTime = Now;

    FBBBCharacterAnimationRequest Request;
    Request.Montage = FireMontage;
    Request.bRestartIfPlaying = true;

    Weapon.SubmitMontage(Request);

    Weapon.PlayFireSound(FireSound, MuzzleSocketName);

    const FVector2D RecoilImpulse(
        VerticalRecoilAmount + FMath::FRandRange(0.0f, VerticalRecoilRandom),
        HorizontalRecoilAmount + FMath::FRandRange(-HorizontalRecoilRandom, HorizontalRecoilRandom));

    Weapon.SubmitRecoil(RecoilImpulse, RecoilRecoverySpeed);
    return true;
}

void UBBBSingleProjectileFireFragment::Present(
    ABBBWeaponActor &Weapon) const
{

    FBBBCharacterAnimationRequest Request;
    Request.Montage = FireMontage;
    Request.bRestartIfPlaying = true;

    Weapon.SubmitMontage(Request);

    Weapon.PlayFireSound(FireSound, MuzzleSocketName);
}
