#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/BBBSingleProjectileFireFragment.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

namespace
{
bool SpawnProjectile(
    ABBBEquipmentPresentationActor &PresentationActor,
    TSubclassOf<ABBBBulletActor> BulletActorClass,
    const FTransform &SpawnTransform,
    float MuzzleSpeed)
{
    APawn *OwnerPawn = Cast<APawn>(PresentationActor.GetOwner());
    UWorld *World = PresentationActor.GetWorld();

    if (!ensureMsgf(BulletActorClass && OwnerPawn && World, TEXT("[UBBBE]Projectile spawn dependencies are invalid")))
    {
        return false;
    }

    FActorSpawnParameters Parameters;
    Parameters.Owner = OwnerPawn;
    Parameters.Instigator = OwnerPawn;
    Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABBBBulletActor *Bullet = World->SpawnActor<ABBBBulletActor>(
        BulletActorClass,
        SpawnTransform,
        Parameters);

    if (!ensureMsgf(Bullet, TEXT("[UBBBE]Projectile spawn failed")))
    {
        return false;
    }

    Bullet->InitializeBullet(
        SpawnTransform.GetUnitAxis(EAxis::X) * MuzzleSpeed,
        OwnerPawn,
        &PresentationActor);

    return true;
}

void PlayFireSound(
    ABBBEquipmentPresentationActor &PresentationActor,
    USoundBase *Sound,
    FName SocketName)
{
    if (!Sound || !PresentationActor.GetWorld())
    {
        return;
    }

    UStaticMeshComponent *EquipmentMesh = PresentationActor.GetEquipmentMesh();
    const FVector SoundLocation = EquipmentMesh && EquipmentMesh->DoesSocketExist(SocketName)
        ? EquipmentMesh->GetSocketLocation(SocketName)
        : PresentationActor.GetActorLocation();

    UGameplayStatics::SpawnSoundAtLocation(PresentationActor.GetWorld(), Sound, SoundLocation);
}
}

FBBBSingleProjectileFireFragment::FBBBSingleProjectileFireFragment()
{
    BulletActorClass = ABBBBulletActor::StaticClass();
}

UBBBFireRuntimeData *FBBBSingleProjectileFireFragment::InitializeRuntimeData(UObject &Outer) const
{
    return NewObject<UBBBFireRuntimeData>(&Outer);
}

bool FBBBSingleProjectileFireFragment::Fire(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor,
    UBBBFireRuntimeData &RuntimeData) const
{
    UWorld *World = PresentationActor.GetWorld();
    UStaticMeshComponent *EquipmentMesh = PresentationActor.GetEquipmentMesh();

    if (!ensureMsgf(World && EquipmentMesh, TEXT("[UBBBE]Projectile fire dependencies are invalid")))
    {
        return false;
    }

    const float CurrentTime = World->GetTimeSeconds();
    if (CurrentTime - RuntimeData.LastFireTime < FireInterval)
    {
        return false;
    }

    if (!ensureMsgf(EquipmentMesh->DoesSocketExist(MuzzleSocketName), TEXT("[UBBBE]Projectile fire muzzle socket is missing")))
    {
        return false;
    }

    const FTransform MuzzleTransform = EquipmentMesh->GetSocketTransform(MuzzleSocketName, RTS_World);
    if (!SpawnProjectile(PresentationActor, BulletActorClass, MuzzleTransform, MuzzleSpeed))
    {
        return false;
    }

    RuntimeData.LastFireTime = CurrentTime;

    if (FireMontage)
    {
        FBBBCharacterAnimationRequest Request;
        Request.Montage = FireMontage;
        CharacterAPI.QueueMontage(Request);
    }

    PlayFireSound(PresentationActor, FireSound, MuzzleSocketName);

    const FVector2D RecoilImpulse(
        VerticalRecoilAmount + FMath::FRandRange(-VerticalRecoilRandom, VerticalRecoilRandom),
        HorizontalRecoilAmount + FMath::FRandRange(-HorizontalRecoilRandom, HorizontalRecoilRandom));

    CharacterAPI.SubmitCameraRecoil(RecoilImpulse, RecoilRecoverySpeed);
    return true;
}

void FBBBSingleProjectileFireFragment::Present(
    FBBBCharacterExternalAPI &CharacterAPI,
    ABBBEquipmentPresentationActor &PresentationActor) const
{
    if (FireMontage)
    {
        FBBBCharacterAnimationRequest Request;
        Request.Montage = FireMontage;
        CharacterAPI.QueueMontage(Request);
    }

    PlayFireSound(PresentationActor, FireSound, MuzzleSocketName);
}
