#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/BBBEquipmentFireSystem.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

bool FBBBEquipmentFireSystem::Fire(
    ABBBEquipmentInstance &Instance,
    const int32 Sequence) const
{
    UWorld *World = Instance.GetWorld();
    USkeletalMeshComponent *WeaponMesh = Instance.EquipmentSkeletalMesh;
    const UBBBEquipmentDefinition *Definition = Instance.Definition;
    if (!ensureMsgf(
        Instance.bIsActive && World && WeaponMesh && Definition,
        TEXT("[UBBBE]Equipment fire dependencies are invalid")))
    {
        return false;
    }

    const FBBBEquipmentFireConfig &Config = Definition->FireConfig;
    const float CurrentTime = World->GetTimeSeconds();
    if (!ensureMsgf(
        !Config.MuzzleSocketName.IsNone() && WeaponMesh->DoesSocketExist(Config.MuzzleSocketName),
        TEXT("[UBBBE]Equipment muzzle socket '%s' is missing"),
        *Config.MuzzleSocketName.ToString()))
    {
        return false;
    }

    const FTransform MuzzleTransform = WeaponMesh->GetSocketTransform(Config.MuzzleSocketName, RTS_World);
    if (!Instance.bIsMirror && Config.BulletActorClass)
    {
        APawn *OwnerPawn = Cast<APawn>(Instance.GetOwner());
        if (!ensureMsgf(OwnerPawn, TEXT("[UBBBE]Projectile fire requires a pawn holder")))
        {
            return false;
        }

        FActorSpawnParameters Parameters;
        Parameters.Owner = OwnerPawn;
        Parameters.Instigator = OwnerPawn;
        Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABBBBulletActor *Bullet = World->SpawnActor<ABBBBulletActor>(
            Config.BulletActorClass,
            MuzzleTransform,
            Parameters);
        if (!ensureMsgf(Bullet, TEXT("[UBBBE]Equipment projectile spawn failed")))
        {
            return false;
        }

        Bullet->InitializeBullet(
            MuzzleTransform.GetUnitAxis(EAxis::X) * Config.MuzzleSpeed,
            OwnerPawn,
            &Instance);
    }

    Instance.RuntimeData.Fire.LastFireTimeSeconds = CurrentTime;
    if (Config.FireSound)
    {
        UGameplayStatics::SpawnSoundAtLocation(World, Config.FireSound, MuzzleTransform.GetLocation());
    }

    if (Config.FireMontage
        && ensureMsgf(Instance.CharacterAPI, TEXT("[UBBBE]Equipment has no character external API")))
    {
        Instance.CharacterAPI->SubmitEquipmentMontage(
            Config.FireMontage,
            1.0f, Sequence);
    }

    if (!Instance.bIsMirror)
    {
        const FVector2D RecoilImpulse(
            Config.VerticalRecoilAmount + FMath::FRandRange(-Config.VerticalRecoilRandom, Config.VerticalRecoilRandom),
            Config.HorizontalRecoilAmount + FMath::FRandRange(-Config.HorizontalRecoilRandom, Config.HorizontalRecoilRandom));
        Instance.RuntimeData.Ammo.LoadedAmmo--;
        if (ensureMsgf(Instance.CharacterAPI, TEXT("[UBBBE]Missing character recoil sink")))
        {
            Instance.CharacterAPI->SubmitEquipmentRecoil(RecoilImpulse, Config.RecoilRecoverySpeed);
        }
    }

    Instance.AnimationSystem.RecordFire();
    return true;
}
