
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

ABBBBulletActor::ABBBBulletActor()
{
    // 子弹演员只负责碰撞表现和投射物移动
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
    // 初始化弹道拖尾和生命周期
    if (TrailComponent && TrailEffect)
    {
        TrailComponent->SetAsset(TrailEffect);
        TrailComponent->Activate(true);
    }
    if (LifeSeconds > 0.0f)
    {
        SetLifeSpan(LifeSeconds);
    }
    // 绑定投射物停止后的销毁处理
    if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &ABBBBulletActor::HandleProjectileStop);
    }
}

void ABBBBulletActor::InitializeBullet(FVector InitialVelocity, AActor *IgnoredOwner, AActor *IgnoredEquipmentActor)
{
    // 忽略发射者和装备演员避免初始碰撞
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
    // 根据初速度启动投射物移动
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
    // 离开场景时停止拖尾表现
    if (TrailComponent)
    {
        TrailComponent->DeactivateImmediate();
    }
    Super::EndPlay(EndPlayReason);
}

void ABBBBulletActor::HandleProjectileStop(const FHitResult &ImpactResult)
{

    // 投射物停止后销毁子弹演员
    Destroy();
}
