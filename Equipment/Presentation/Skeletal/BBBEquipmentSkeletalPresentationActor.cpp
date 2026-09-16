#include "BBBWork/UBBBNexus/Equipment/Presentation/Skeletal/BBBEquipmentSkeletalPresentationActor.h"

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
