#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"

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

void UBBBAnimInstance::SubmitEquipmentActionMontage(const FBBBEquipmentActionEvent &Event)
{
    if (!ensureMsgf(Event.Presentation.Montage, TEXT("[UBBBC]Equipment action presentation montage is null")))
    {
        return;
    }

    EquipmentActionType = Event.ActionType;
    EquipmentActionSequence = Event.Sequence;
    EquipmentActionDuration = Event.DurationSeconds;
    EquipmentActionMontage = Event.Presentation.Montage;
    EquipmentActionPlayRate = Event.Presentation.PlayRate;

    ExecuteEquipmentActionMontage(
        EquipmentActionType,
        EquipmentActionMontage,
        EquipmentActionPlayRate);
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
