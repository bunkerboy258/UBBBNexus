#include "BBBWork/UBBBNexus/Equipment/Presentation/Skeletal/BBBEquipmentSkeletalPresentationActor.h"

#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/BBBEquipmentAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

ABBBEquipmentSkeletalPresentationActor::ABBBEquipmentSkeletalPresentationActor()
{
    EquipmentSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentSkeletalMesh"));
    EquipmentSkeletalMesh->SetupAttachment(GetRootComponent());
    EquipmentSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EquipmentSkeletalMesh->SetGenerateOverlapEvents(false);
}

USkeletalMeshComponent *ABBBEquipmentSkeletalPresentationActor::GetEquipmentSkeletalMesh() const
{
    return EquipmentSkeletalMesh;
}

USceneComponent *ABBBEquipmentSkeletalPresentationActor::GetEquipmentAttachmentComponent() const
{
    return EquipmentSkeletalMesh;
}

void ABBBEquipmentSkeletalPresentationActor::NotifyFire(const float WorldTimeSeconds)
{
    if (!ensureMsgf(EquipmentSkeletalMesh, TEXT("[UBBBE]Equipment skeletal mesh is unavailable during fire notification")))
    {
        return;
    }

    UBBBEquipmentAnimInstance *AnimInstance = Cast<UBBBEquipmentAnimInstance>(
        EquipmentSkeletalMesh->GetAnimInstance());
    if (!ensureMsgf(AnimInstance, TEXT("[UBBBE]Equipment skeletal mesh does not use UBBBEquipmentAnimInstance")))
    {
        return;
    }

    AnimInstance->NotifyFire(WorldTimeSeconds);
}

void ABBBEquipmentSkeletalPresentationActor::NotifyReloadStarted(
    const float WorldTimeSeconds,
    const float DurationSeconds)
{
    if (!ensureMsgf(EquipmentSkeletalMesh, TEXT("[UBBBE]Equipment skeletal mesh is unavailable during reload notification")))
    {
        return;
    }

    UBBBEquipmentAnimInstance *AnimInstance = Cast<UBBBEquipmentAnimInstance>(
        EquipmentSkeletalMesh->GetAnimInstance());
    if (!ensureMsgf(AnimInstance, TEXT("[UBBBE]Equipment skeletal mesh does not use UBBBEquipmentAnimInstance")))
    {
        return;
    }

    AnimInstance->NotifyReloadStarted(WorldTimeSeconds, DurationSeconds);
}

void ABBBEquipmentSkeletalPresentationActor::NotifyReloadCompleted(const float WorldTimeSeconds)
{
    if (!ensureMsgf(EquipmentSkeletalMesh, TEXT("[UBBBE]Equipment skeletal mesh is unavailable during reload completion")))
    {
        return;
    }

    UBBBEquipmentAnimInstance *AnimInstance = Cast<UBBBEquipmentAnimInstance>(
        EquipmentSkeletalMesh->GetAnimInstance());
    if (!ensureMsgf(AnimInstance, TEXT("[UBBBE]Equipment skeletal mesh does not use UBBBEquipmentAnimInstance")))
    {
        return;
    }

    AnimInstance->NotifyReloadCompleted(WorldTimeSeconds);
}
