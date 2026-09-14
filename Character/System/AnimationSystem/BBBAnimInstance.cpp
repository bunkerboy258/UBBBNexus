#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
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
    MainHandEquipmentInstance = Facts.MainHandEquipmentInstance;

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

bool UBBBAnimInstance::TryGetCharacterBoneWorldTransform(
    const FName BoneName,
    FTransform &OutBoneWorld) const
{
    OutBoneWorld = FTransform::Identity;

    USkeletalMeshComponent *CharacterMesh = GetOwningComponent();
    if (!ensureMsgf(CharacterMesh, TEXT("[UBBBC]Character animation owning mesh is null during bone query")))
    {
        return false;
    }

    if (!ensureMsgf(
        BoneName != NAME_None
        && CharacterMesh->GetBoneIndex(BoneName) != INDEX_NONE,
        TEXT("[UBBBC]Character bone '%s' is invalid"),
        *BoneName.ToString()))
    {
        return false;
    }

    OutBoneWorld = CharacterMesh->GetBoneTransform(BoneName, RTS_World);
    return true;
}

//------------------------------------------------------------------------------

bool UBBBAnimInstance::TryGetCurrentLeftHandIKSourceData(
    const FName SocketName,
    const FName ReferenceBoneName,
    FTransform &OutSocketComponentSpace,
    FVector &OutSocketOffset,
    FTransform &OutEquipmentWorld,
    FTransform &OutReferenceBoneWorld) const
{
    OutSocketComponentSpace = FTransform::Identity;
    OutSocketOffset = FVector::ZeroVector;
    OutEquipmentWorld = FTransform::Identity;
    OutReferenceBoneWorld = FTransform::Identity;

    if (!MainHandEquipmentInstance)
    {
        return false;
    }

    if (!MainHandEquipmentInstance->TryGetSocketTransforms(
        SocketName,
        OutSocketComponentSpace,
        OutEquipmentWorld))
    {
        return false;
    }

    if (!TryGetCharacterBoneWorldTransform(
        ReferenceBoneName,
        OutReferenceBoneWorld))
    {
        return false;
    }

    OutSocketOffset = MainHandEquipmentInstance->GetLeftHandIKSocketOffset();
    return true;
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
