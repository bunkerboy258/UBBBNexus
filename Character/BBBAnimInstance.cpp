#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"

#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

UBBBAnimInstance *UBBBAnimInstance::GetBBBMainAnimInstanceThreadSafe() const
{
    UBBBAnimInstance *MainAnimInstance = Cast<UBBBAnimInstance>(Blueprint_GetMainAnimInstance());
    if (MainAnimInstance)
    {
        return MainAnimInstance;
    }

    return const_cast<UBBBAnimInstance *>(this);
}

//------------------------------------------------------------------------------

void UBBBAnimInstance::PublishAnimationFacts(
    const FBBBCharacterAnimationFacts &Facts)
{
    AnimationFacts = Facts;

    SourceActorLocation = Facts.ActorLocation;
    SourceActorRotation = Facts.ActorRotation;
    SourceVelocity = Facts.Velocity;
    SourceLastUpdateVelocity = Facts.LastUpdateVelocity;
    SourceAcceleration = Facts.Acceleration;
    SourceMovementMode = Facts.MovementMode;

    SourceGroundFriction = Facts.GroundFriction;
    SourceBrakingFriction = Facts.BrakingFriction;
    SourceBrakingFrictionFactor = Facts.BrakingFrictionFactor;
    SourceBrakingDecelerationWalking = Facts.BrakingDecelerationWalking;
    bSourceUseSeparateBrakingFriction = Facts.bUseSeparateBrakingFriction;
    SourceGravityZ = Facts.GravityZ;

    bSourceMovingOnGround = Facts.bIsMovingOnGround;
    bSourceCrouching = Facts.bIsCrouching;
    GroundDistance = Facts.GroundDistance;
}

//------------------------------------------------------------------------------

UBBBEquipmentAnimInstance *UBBBAnimInstance::TryGetWeaponAnimInstance() const
{
    return GetBBBMainAnimInstanceThreadSafe()->WeaponAnimInstance.Get();
}

FTransform UBBBAnimInstance::TryGetWeaponAimSourceLocalTransform() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->GetAimSourceLocalTransform();
    }

    return FTransform::Identity;
}

FVector UBBBAnimInstance::TryGetWeaponLeftHandTargetHandRSpace() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->GetLeftHandTargetHandRSpace();
    }

    return FVector::ZeroVector;
}

bool UBBBAnimInstance::TryHasWeaponLeftHandTarget() const
{
    const UBBBEquipmentAnimInstance *Weapon = TryGetWeaponAnimInstance();
    if (Weapon)
    {
        return Weapon->HasLeftHandTarget();
    }

    return false;
}

void UBBBAnimInstance::BindWeaponAnimInstance(UBBBEquipmentAnimInstance *InWeaponAnimInstance)
{
    GetBBBMainAnimInstanceThreadSafe()->WeaponAnimInstance = InWeaponAnimInstance;
}

void UBBBAnimInstance::PlayMovementActionMontage(UAnimMontage *Montage, float PlayRate)
{
    if (!ensureMsgf(Montage, TEXT("[UBBBC]Movement action montage is null")))
    {
        return;
    }

    const float PlayedLength = Montage_Play(Montage, FMath::Max(PlayRate, 0.01f));
    ensureMsgf(
        PlayedLength > 0.0f,
        TEXT("[UBBBC]Movement action montage '%s' failed to play"),
        *Montage->GetPathName());
}
