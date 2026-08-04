
#include "BBBWork/UBBBNexus/Item/Template/Weapon/Projectile/BBBBulletActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

ABBBBulletActor::ABBBBulletActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetReplicateMovement(false);
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    SetRootComponent(CollisionComponent);
    CollisionComponent->InitSphereRadius(2.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
    VisualMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMeshComponent"));
    VisualMeshComponent->SetupAttachment(CollisionComponent);
    VisualMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailComponent"));
    TrailComponent->SetupAttachment(CollisionComponent);
    TrailComponent->bAutoActivate = true;
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
    ProjectileMovementComponent->bAutoActivate = false;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
}

void ABBBBulletActor::BeginPlay()
{
    Super::BeginPlay();
    if (TrailComponent && TrailEffect)
    {
        TrailComponent->SetAsset(TrailEffect);
        TrailComponent->Activate(true);
    }
    if (LifeSeconds > 0.0f)
    {
        SetLifeSpan(LifeSeconds);
    }
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &ABBBBulletActor::HandleProjectileStop);
    }
}

void ABBBBulletActor::InitializeBullet(FVector InitialVelocity, AActor *IgnoredOwner, AActor *IgnoredEquipmentActor)
{
    if (CollisionComponent)
    {
        if (IgnoredOwner)
        {
            CollisionComponent->IgnoreActorWhenMoving(IgnoredOwner, true);
        }
        if (IgnoredEquipmentActor)
        {
            CollisionComponent->IgnoreActorWhenMoving(IgnoredEquipmentActor, true);
        }
    }
    if (ProjectileMovementComponent)
    {
        const float Speed = InitialVelocity.Size();
        ProjectileMovementComponent->InitialSpeed = Speed;
        ProjectileMovementComponent->MaxSpeed = Speed;
        ProjectileMovementComponent->Velocity = InitialVelocity;
        ProjectileMovementComponent->Activate(true);
    }
}

void ABBBBulletActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (TrailComponent)
    {
        TrailComponent->DeactivateImmediate();
    }
    Super::EndPlay(EndPlayReason);
}

void ABBBBulletActor::HandleProjectileStop(const FHitResult &ImpactResult)
{

    Destroy();
}
