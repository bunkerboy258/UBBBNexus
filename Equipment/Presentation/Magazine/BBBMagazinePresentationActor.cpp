#include "BBBWork/UBBBNexus/Equipment/Presentation/Magazine/BBBMagazinePresentationActor.h"

#include "Components/StaticMeshComponent.h"

ABBBMagazinePresentationActor::ABBBMagazinePresentationActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetReplicateMovement(false);

    MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineMesh"));
    SetRootComponent(MagazineMesh);
    PrepareForAttachment();
}

//------------------------------------------------------------------------------

void ABBBMagazinePresentationActor::PrepareForAttachment()
{
    MagazineMesh->SetSimulatePhysics(false);
    MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MagazineMesh->SetGenerateOverlapEvents(false);
}

//------------------------------------------------------------------------------

void ABBBMagazinePresentationActor::Drop(float LifeSeconds)
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    MagazineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MagazineMesh->SetCollisionObjectType(ECC_PhysicsBody);
    MagazineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    MagazineMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    MagazineMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    MagazineMesh->SetSimulatePhysics(true);
    MagazineMesh->WakeAllRigidBodies();

    SetLifeSpan(FMath::Max(LifeSeconds, 0.1f));
}
