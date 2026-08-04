#include "BBBWork/UBBBNexus/Item/Equipment/Fire/BBBSingleProjectileFireOperation.h"

#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/Commands/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Item/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Fire/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Projectile/BBBBulletActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

UBBBSingleProjectileFireOperation::UBBBSingleProjectileFireOperation()
{
    BulletActorClass = ABBBBulletActor::StaticClass();
}

bool UBBBSingleProjectileFireOperation::Fire(
    ABBBEquipmentActor &EquipmentActor,
    UBBBFireRuntimeData &RuntimeData) const
{
    UWorld *World = EquipmentActor.GetWorld();
    UStaticMeshComponent *EquipmentMesh = EquipmentActor.GetEquipmentMesh();

    if (!ensureMsgf(World && EquipmentMesh, TEXT("[UBBBI]Projectile fire dependencies are invalid")))
    {
        return false;
    }

    const float CurrentTime = World->GetTimeSeconds();
    if (CurrentTime - RuntimeData.LastFireTime < FireInterval)
    {
        return false;
    }

    if (!ensureMsgf(EquipmentMesh->DoesSocketExist(MuzzleSocketName), TEXT("[UBBBI]Projectile fire muzzle socket is missing")))
    {
        return false;
    }

    const FTransform MuzzleTransform = EquipmentMesh->GetSocketTransform(MuzzleSocketName, RTS_World);
    if (!EquipmentActor.SpawnProjectile(BulletActorClass, MuzzleTransform, MuzzleSpeed))
    {
        return false;
    }

    RuntimeData.LastFireTime = CurrentTime;

    FBBBCharacterAnimationRequest Request;
    Request.Montage = FireMontage;
    EquipmentActor.QueueOperationMontage(Request);
    EquipmentActor.PlayOperationSound(FireSound, MuzzleSocketName);

    const FVector2D RecoilImpulse(
        VerticalRecoilAmount + FMath::FRandRange(-VerticalRecoilRandom, VerticalRecoilRandom),
        HorizontalRecoilAmount + FMath::FRandRange(-HorizontalRecoilRandom, HorizontalRecoilRandom));

    EquipmentActor.SubmitRecoil(RecoilImpulse, RecoilRecoverySpeed);
    return true;
}

void UBBBSingleProjectileFireOperation::Present(ABBBEquipmentActor &EquipmentActor) const
{
    FBBBCharacterAnimationRequest Request;
    Request.Montage = FireMontage;
    EquipmentActor.QueueOperationMontage(Request);
    EquipmentActor.PlayOperationSound(FireSound, MuzzleSocketName);
}
