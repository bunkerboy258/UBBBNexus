#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "Components/SkeletalMeshComponent.h"

bool UBBBEquipmentAnimInstance::BindCharacterMesh(USkeletalMeshComponent *InCharacterMesh)
{
    CharacterMesh = InCharacterMesh;
    if (!InCharacterMesh)
    {
        AimSourceLocalTransform = FTransform::Identity;
        bHasValidAimSource = false;
        return true;
    }

    if (!ensureMsgf(HasLeftHandTarget(), TEXT("[UBBBE]Weapon left hand socket or character hand_r bone is missing")))
    {
        CharacterMesh.Reset();
        return false;
    }

    return true;
}

bool UBBBEquipmentAnimInstance::HasMainHandEquipment() const
{
    return CharacterMesh.IsValid();
}

bool UBBBEquipmentAnimInstance::HasLeftHandTarget() const
{
    const USkeletalMeshComponent *OwnerMesh = CharacterMesh.Get();
    const USkeletalMeshComponent *WeaponMesh = GetSkelMeshComponent();
    return OwnerMesh && WeaponMesh
        && OwnerMesh->GetBoneIndex(TEXT("hand_r")) != INDEX_NONE
        && WeaponMesh->DoesSocketExist(LeftHandSocketName);
}

FVector UBBBEquipmentAnimInstance::GetLeftHandTargetHandRSpace() const
{
    if (!HasLeftHandTarget())
    {
        return FVector::ZeroVector;
    }

    const USkeletalMeshComponent *WeaponMesh = GetSkelMeshComponent();
    const FVector SocketPosition = WeaponMesh->GetSocketTransform(LeftHandSocketName, RTS_Component).GetLocation();
    const FVector TargetWorld = WeaponMesh->GetComponentTransform().TransformPosition(SocketPosition + LeftHandSocketOffset);
    return CharacterMesh->GetBoneTransform(TEXT("hand_r"), RTS_World).InverseTransformPosition(TargetWorld);
}

void UBBBEquipmentAnimInstance::BindAimSource(const FTransform &InAimSourceLocalTransform, bool bInHasValidAimSource)
{
    AimSourceLocalTransform = FTransform::Identity;
    bHasValidAimSource = false;
    if (!bInHasValidAimSource)
    {
        return;
    }

    if (!ensureMsgf(InAimSourceLocalTransform.IsValid(), TEXT("[UBBBE]Weapon aim source transform is invalid")))
    {
        return;
    }

    AimSourceLocalTransform = InAimSourceLocalTransform;
    bHasValidAimSource = true;
}

void UBBBEquipmentAnimInstance::PublishEquipmentAction(const FBBBEquipmentActionEvent &Event)
{
    EquipmentActionType = Event.ActionType;
    EquipmentActionSequence = Event.Sequence;
    EquipmentActionDuration = Event.DurationSeconds;
    EquipmentActionMontage = Event.Presentation.Montage;
    EquipmentActionPlayRate = Event.Presentation.PlayRate;
}

void UBBBEquipmentAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (UWorld* World = GetWorld())
    {
        AnimationState.CurrentWorldTimeSeconds = World->GetTimeSeconds();
    }

    AnimationState.FireElapsedSeconds = FMath::Max(
        AnimationState.CurrentWorldTimeSeconds - AnimationState.LastFireTimeSeconds,
        0.0f
    );
    AnimationState.ReloadElapsedSeconds = FMath::Max(
        AnimationState.CurrentWorldTimeSeconds - AnimationState.ReloadStartTimeSeconds,
        0.0f
    );
    AnimationState.bFireActive = AnimationState.FireSequence > 0 && AnimationState.FireElapsedSeconds < 0.333333f;
}

void UBBBEquipmentAnimInstance::NotifyFire(const float WorldTimeSeconds)
{
    AnimationState.FireSequence++;
    AnimationState.LastFireTimeSeconds = WorldTimeSeconds;
    AnimationState.bFireActive = true;
}

void UBBBEquipmentAnimInstance::NotifyReloadStarted(
    const float WorldTimeSeconds,
    const float DurationSeconds)
{
    AnimationState.ReloadSequence++;
    AnimationState.bIsReloading = true;
    AnimationState.ReloadStartTimeSeconds = WorldTimeSeconds;
    AnimationState.ReloadDurationSeconds = FMath::Max(DurationSeconds, 0.01f);
}

void UBBBEquipmentAnimInstance::NotifyReloadCompleted(const float WorldTimeSeconds)
{
    AnimationState.bIsReloading = false;
}
