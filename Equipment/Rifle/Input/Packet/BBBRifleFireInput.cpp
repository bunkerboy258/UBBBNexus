#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleFireInput.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

bool FBBBRifleFireInput::IsValid() const
{
    return Sequence != INDEX_NONE;
}

bool FBBBRifleFireInput::CanApply(const FBBBRifleInputContext &Context) const
{
    UWorld *World = Context.GetWorld();
    if (!World || Context.RuntimeData.bIsReloading || Context.RuntimeData.LoadedAmmo <= 0)
    {
        return false;
    }

    if (!Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName))
    {
        ensureMsgf(
            false,
            TEXT("步枪网格缺少枪口插槽 %s"),
            *Context.Definition.MuzzleSocketName.ToString());
        return false;
    }

    return World->GetTimeSeconds() - Context.RuntimeData.LastFireTimeSeconds
        >= Context.Definition.FireInterval;
}

void FBBBRifleFireInput::Apply(FBBBRifleInputContext &Context) const
{
    UWorld *World = Context.GetWorld();
    if (!World)
    {
        return;
    }

    const FTransform MuzzleTransform = Context.WeaponMesh.GetSocketTransform(
        Context.Definition.MuzzleSocketName,
        RTS_World);
    APawn *OwnerPawn = Cast<APawn>(Context.Equipment.GetOwner());
    if (Context.Definition.BulletActorClass && OwnerPawn)
    {
        FActorSpawnParameters Parameters;
        Parameters.Owner = OwnerPawn;
        Parameters.Instigator = OwnerPawn;
        Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABBBBulletActor *Bullet = World->SpawnActor<ABBBBulletActor>(
            Context.Definition.BulletActorClass,
            MuzzleTransform,
            Parameters);
        if (Bullet)
        {
            Bullet->InitializeBullet(
                MuzzleTransform.GetUnitAxis(EAxis::X) * Context.Definition.MuzzleSpeed,
                OwnerPawn,
                &Context.Equipment);
        }
    }

    Context.PlayFireSound();
    Context.PlayEquipmentMontage(Context.Definition.EquipmentFireMontage);

    FBBBCameraPacket CameraPacket;
    CameraPacket.Impulse = FVector2D(
        Context.Definition.VerticalRecoilAmount
            + FMath::FRandRange(
                -Context.Definition.VerticalRecoilRandom,
                Context.Definition.VerticalRecoilRandom),
        Context.Definition.HorizontalRecoilAmount
            + FMath::FRandRange(
                -Context.Definition.HorizontalRecoilRandom,
                Context.Definition.HorizontalRecoilRandom));
    CameraPacket.RecoverySpeed = Context.Definition.RecoilRecoverySpeed;
    Context.Character.SubmitInput(CameraPacket);

    Context.RuntimeData.LoadedAmmo--;
    Context.RuntimeData.FireSequence = Sequence;
    Context.RuntimeData.LastFireTimeSeconds = World->GetTimeSeconds();
    Context.Character.SubmitInput(FBBBFireFactPacket{
        Context.Equipment.GetEquipmentId(),
        Sequence,
        Context.RuntimeData.LoadedAmmo});
}
