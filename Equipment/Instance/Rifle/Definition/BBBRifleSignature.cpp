#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleSignature.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

void FBBBRifleSignature::Initialize(FState &State, const UBBBRifleDefinition &Definition)
{
    State.AmmoCapacity = FMath::Max(Definition.AmmoCapacity, 1);
    State.LoadedAmmo = State.AmmoCapacity;
}

void FBBBRifleSignature::Parse(FState &State, FInputFrame &InputFrame, FBBBEquipmentRuntimeContext &Context)
{
    const UBBBRifleDefinition *Definition = Cast<UBBBRifleDefinition>(Context.Equipment.GetDefinition());
    if (!Definition)
    {
        return;
    }

    FBBBRifleFactInput FactInput;
    if (InputFrame.Consume(FactInput))
    {
        State.LoadedAmmo = FactInput.Fact.LoadedAmmo;
        State.FireSequence = FactInput.Fact.Sequence;
    }

    FBBBRifleEquipInput EquipInput;
    if (InputFrame.Consume(EquipInput) && Definition->EquipMontage)
    {
        BBBCharacterMontageInput::Submit(Context.Character, *Definition->EquipMontage, 1.0f, EquipInput.Sequence, false);
    }

    FBBBRifleReloadInput ReloadInput;
    if (InputFrame.Consume(ReloadInput)
        && !State.bIsReloading
        && State.LoadedAmmo < State.AmmoCapacity
        && Definition->ReloadMontage)
    {
        BBBCharacterMontageInput::Submit(Context.Character, *Definition->ReloadMontage, 1.0f, ReloadInput.Sequence, true);
        State.bIsReloading = true;
        State.bMagazineDetached = false;
        State.ReloadSequence = ReloadInput.Sequence;
        Context.Character.SubmitInput(FBBBReloadStartedFactPacket{
            Context.Equipment.GetEquipmentId(),
            ReloadInput.Sequence,
            State.LoadedAmmo});
    }

    FBBBRifleDetachMagazineInput DetachInput;
    if (InputFrame.Consume(DetachInput)
        && State.bIsReloading
        && !State.bMagazineDetached
        && (DetachInput.Sequence == INDEX_NONE || DetachInput.Sequence == State.ReloadSequence))
    {
        State.LoadedAmmo = 0;
        State.bMagazineDetached = true;
        Context.Character.SubmitInput(FBBBMagazineDetachedFactPacket{
            Context.Equipment.GetEquipmentId(),
            State.ReloadSequence,
            State.LoadedAmmo});
    }

    FBBBRifleLoadMagazineInput LoadInput;
    if (InputFrame.Consume(LoadInput)
        && State.bIsReloading
        && State.bMagazineDetached
        && (LoadInput.Sequence == INDEX_NONE || LoadInput.Sequence == State.ReloadSequence))
    {
        State.LoadedAmmo = State.AmmoCapacity;
        State.bIsReloading = false;
        Context.Character.SubmitInput(FBBBMagazineLoadedFactPacket{
            Context.Equipment.GetEquipmentId(),
            State.ReloadSequence,
            State.LoadedAmmo});
    }

    FBBBRifleCancelReloadInput CancelInput;
    if (InputFrame.Consume(CancelInput)
        && State.bIsReloading
        && (CancelInput.Sequence == INDEX_NONE || CancelInput.Sequence == State.ReloadSequence))
    {
        State.bIsReloading = false;
        Context.Character.SubmitInput(FBBBReloadCancelledFactPacket{
            Context.Equipment.GetEquipmentId(),
            State.ReloadSequence,
            State.LoadedAmmo});
    }

    FBBBRifleFireInput FireInput;
    if (InputFrame.Consume(FireInput)
        && !State.bIsReloading
        && State.LoadedAmmo > 0
        && Context.WeaponMesh.DoesSocketExist(Definition->MuzzleSocketName)
        && Context.WeaponMesh.GetWorld()->GetTimeSeconds() - State.LastFireTimeSeconds >= Definition->FireInterval)
    {
        const FTransform MuzzleTransform = Context.WeaponMesh.GetSocketTransform(Definition->MuzzleSocketName, RTS_World);
        UWorld *World = Context.WeaponMesh.GetWorld();
        if (!Context.bIsMirror && Definition->BulletActorClass && World)
        {
            APawn *OwnerPawn = Cast<APawn>(Context.Equipment.GetOwner());
            if (OwnerPawn)
            {
                FActorSpawnParameters Parameters;
                Parameters.Owner = OwnerPawn;
                Parameters.Instigator = OwnerPawn;
                Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                ABBBBulletActor *Bullet = World->SpawnActor<ABBBBulletActor>(Definition->BulletActorClass, MuzzleTransform, Parameters);
                if (Bullet)
                {
                    Bullet->InitializeBullet(MuzzleTransform.GetUnitAxis(EAxis::X) * Definition->MuzzleSpeed, OwnerPawn, &Context.Equipment);
                }
            }
        }

        if (Definition->FireSound && World)
        {
            UGameplayStatics::SpawnSoundAtLocation(World, Definition->FireSound, MuzzleTransform.GetLocation());
        }

        if (Definition->FireMontage)
        {
            BBBCharacterMontageInput::Submit(Context.Character, *Definition->FireMontage, 1.0f, FireInput.Sequence, false);
        }

        if (!Context.bIsMirror)
        {
            State.LoadedAmmo--;
            FBBBCameraPacket CameraPacket;
            CameraPacket.Impulse = FVector2D(
                Definition->VerticalRecoilAmount + FMath::FRandRange(-Definition->VerticalRecoilRandom, Definition->VerticalRecoilRandom),
                Definition->HorizontalRecoilAmount + FMath::FRandRange(-Definition->HorizontalRecoilRandom, Definition->HorizontalRecoilRandom));
            CameraPacket.RecoverySpeed = Definition->RecoilRecoverySpeed;
            Context.Character.SubmitInput(CameraPacket);
        }

        State.FireSequence = FireInput.Sequence;
        State.LastFireTimeSeconds = World ? World->GetTimeSeconds() : State.LastFireTimeSeconds;
        Context.Character.SubmitInput(FBBBFireFactPacket{
            Context.Equipment.GetEquipmentId(),
            FireInput.Sequence,
            State.LoadedAmmo});
    }
}
