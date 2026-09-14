#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "Components/SceneComponent.h"
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
    LeftHandIKOffsetRightHand = Facts.LeftHandIKOffsetRightHand;
    bHasValidLeftHandIKTarget = Facts.bHasValidLeftHandIKTarget;

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

FVector UBBBAnimInstance::GetLiveLeftHandIKTargetRightHandBoneSpace() const
{
    const UBBBEquipmentInstance *EquipmentInstance = AnimationFacts.MainHandEquipmentInstance;
    if (!EquipmentInstance)
    {
        return FVector::ZeroVector;
    }

    USkeletalMeshComponent *CharacterMesh = GetOwningComponent();
    if (!ensureMsgf(CharacterMesh, TEXT("[UBBBC]Character animation owning mesh is null during live left hand IK query")))
    {
        return FVector::ZeroVector;
    }

    ABBBEquipmentPresentationActor *PresentationActor = EquipmentInstance->GetPresentationActor();
    if (!ensureMsgf(PresentationActor, TEXT("[UBBBC]Main hand equipment presentation actor is null during live left hand IK query")))
    {
        return FVector::ZeroVector;
    }

    USceneComponent *EquipmentComponent = PresentationActor->GetEquipmentAttachmentComponent();
    if (!ensureMsgf(EquipmentComponent, TEXT("[UBBBC]Main hand equipment attachment component is null during live left hand IK query")))
    {
        return FVector::ZeroVector;
    }

    const FName AttachmentSocketName = PresentationActor->GetRootComponent()
        ? PresentationActor->GetRootComponent()->GetAttachSocketName()
        : NAME_None;
    const FName ReferenceBoneName = CharacterMesh->GetSocketBoneName(AttachmentSocketName);
    if (!ensureMsgf(
        ReferenceBoneName != NAME_None
        && CharacterMesh->GetBoneIndex(ReferenceBoneName) != INDEX_NONE,
        TEXT("[UBBBC]Live left hand IK reference bone is invalid")))
    {
        return FVector::ZeroVector;
    }

    static const FName LeftHandIKSocketName(TEXT("LeftHand"));
    if (!ensureMsgf(
        EquipmentComponent->DoesSocketExist(LeftHandIKSocketName),
        TEXT("[UBBBC]Live left hand IK socket '%s' is missing"),
        *LeftHandIKSocketName.ToString()))
    {
        return FVector::ZeroVector;
    }

    FTransform SocketComponent = EquipmentComponent->GetSocketTransform(
        LeftHandIKSocketName,
        RTS_Component);
    SocketComponent.AddToTranslation(EquipmentInstance->GetLeftHandIKSocketOffset());

    const FTransform SocketWorld = SocketComponent * EquipmentComponent->GetComponentTransform();
    const FTransform ReferenceBoneWorld = CharacterMesh->GetBoneTransform(ReferenceBoneName, RTS_World);
    return SocketWorld.GetRelativeTransform(ReferenceBoneWorld).GetTranslation();
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
