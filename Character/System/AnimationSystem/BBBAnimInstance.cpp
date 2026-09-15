#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/BBBEquipmentAnimInstance.h"
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

UBBBEquipmentAnimInstance *UBBBAnimInstance::GetWeaponAnimInstance() const
{
    return GetBBBMainAnimInstanceThreadSafe()->WeaponAnimInstance.Get();
}

void UBBBAnimInstance::BindWeaponAnimInstance(UBBBEquipmentAnimInstance *InWeaponAnimInstance)
{
    GetBBBMainAnimInstanceThreadSafe()->WeaponAnimInstance = InWeaponAnimInstance;
}

void UBBBAnimInstance::SubmitEquipmentActionMontage(const FBBBEquipmentActionEvent &Event)
{
    if (!ensureMsgf(Event.Presentation.Montage, TEXT("[UBBBC]Equipment action presentation montage is null")))
    {
        return;
    }

    UBBBEquipmentAnimInstance *Weapon = GetWeaponAnimInstance();
    if (!ensureMsgf(Weapon, TEXT("[UBBBC]Equipment montage has no bound weapon animation instance")))
    {
        return;
    }

    Weapon->PublishEquipmentAction(Event);
    ExecuteEquipmentActionMontage(Event.ActionType, Event.Presentation.Montage, Event.Presentation.PlayRate);
}

//------------------------------------------------------------------------------

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
