#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ABBBEquipmentPresentationActor::ABBBEquipmentPresentationActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetReplicateMovement(false);

    EquipmentRoot = CreateDefaultSubobject<UArrowComponent>(TEXT("EquipmentRoot"));
    SetRootComponent(EquipmentRoot);

    EquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipmentMesh"));
    EquipmentMesh->SetupAttachment(EquipmentRoot);
    EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EquipmentMesh->SetGenerateOverlapEvents(false);
}

UStaticMeshComponent *ABBBEquipmentPresentationActor::GetEquipmentMesh() const
{
    return EquipmentMesh;
}

USceneComponent *ABBBEquipmentPresentationActor::GetEquipmentAttachmentComponent() const
{
    return EquipmentMesh;
}

void ABBBEquipmentPresentationActor::SetMuzzleSocketName(const FName InSocketName)
{
    MuzzleSocketName = InSocketName;
}

FName ABBBEquipmentPresentationActor::GetMuzzleSocketName() const
{
    return MuzzleSocketName;
}

void ABBBEquipmentPresentationActor::NotifyFire(float WorldTimeSeconds)
{
}

void ABBBEquipmentPresentationActor::NotifyReloadStarted(float WorldTimeSeconds, float DurationSeconds)
{
}

void ABBBEquipmentPresentationActor::NotifyReloadCompleted(float WorldTimeSeconds)
{
}
