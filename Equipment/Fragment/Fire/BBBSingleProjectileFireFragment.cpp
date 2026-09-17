#include "BBBWork/UBBBNexus/Equipment/Fragment/Fire/BBBSingleProjectileFireFragment.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Fragment/Definition/BBBEquipmentFragmentContexts.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

FBBBSingleProjectileFireFragment::FBBBSingleProjectileFireFragment()
{
    BulletActorClass = ABBBBulletActor::StaticClass();
}

bool FBBBSingleProjectileFireFragment::CanFire(
    const FBBBEquipmentRuntimeData &Runtime,
    const float WorldTime) const
{
    return Runtime.Ammo.LoadedAmmo > 0
        && WorldTime - Runtime.Fire.LastFireTimeSeconds >= FMath::Max(FireInterval, 0.01f);
}

bool FBBBSingleProjectileFireFragment::Fire(FBBBEquipmentFireContext &Context) const
{
    if (!ensureMsgf(!MuzzleSocketName.IsNone() && Context.WeaponMesh.DoesSocketExist(MuzzleSocketName),
        TEXT("[UBBBE]Equipment muzzle socket '%s' is missing"), *MuzzleSocketName.ToString()))
    {
        return false;
    }

    const FTransform MuzzleTransform = Context.WeaponMesh.GetSocketTransform(MuzzleSocketName, RTS_World);
    if (!Context.bIsMirror && BulletActorClass)
    {
        APawn *OwnerPawn = Cast<APawn>(Context.Instance.GetOwner());
        if (!ensureMsgf(OwnerPawn, TEXT("[UBBBE]Projectile fire requires a pawn holder")))
        {
            return false;
        }

        FActorSpawnParameters Parameters;
        Parameters.Owner = OwnerPawn;
        Parameters.Instigator = OwnerPawn;
        Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABBBBulletActor *Bullet = Context.World.SpawnActor<ABBBBulletActor>(
            BulletActorClass, MuzzleTransform, Parameters);
        if (!ensureMsgf(Bullet, TEXT("[UBBBE]Equipment projectile spawn failed")))
        {
            return false;
        }

        Bullet->InitializeBullet(
            MuzzleTransform.GetUnitAxis(EAxis::X) * MuzzleSpeed,
            OwnerPawn,
            &Context.Instance);
    }

    if (FireSound)
    {
        UGameplayStatics::SpawnSoundAtLocation(&Context.World, FireSound, MuzzleTransform.GetLocation());
    }

    if (FireMontage)
    {
        Context.CharacterAPI.SubmitEquipmentMontage(FireMontage, 1.0f, Context.Sequence);
    }

    if (!Context.bIsMirror)
    {
        const FVector2D RecoilImpulse(
            VerticalRecoilAmount + FMath::FRandRange(-VerticalRecoilRandom, VerticalRecoilRandom),
            HorizontalRecoilAmount + FMath::FRandRange(-HorizontalRecoilRandom, HorizontalRecoilRandom));
        Context.Runtime.Ammo.LoadedAmmo--;
        Context.CharacterAPI.SubmitEquipmentRecoil(RecoilImpulse, RecoilRecoverySpeed);
    }

    Context.Runtime.Fire.LastFireTimeSeconds = Context.World.GetTimeSeconds();
    return true;
}
