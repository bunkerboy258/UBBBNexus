#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleDebugTracer.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ABBBRifleDebugTracer::ABBBRifleDebugTracer()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetCanBeDamaged(false);
    InitialLifeSpan = 1.5f;

    TracerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TracerMesh"));
    SetRootComponent(TracerMesh);
    TracerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TracerMesh->SetGenerateOverlapEvents(false);
    TracerMesh->SetCastShadow(false);
    TracerMesh->SetRelativeScale3D(FVector(6.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> TracerMeshAsset(
        TEXT("/Game/_ThirdParty/Weapons/ModernWeapons/Assets/Projectiles/SM_Modern_Weapons_Projectile_SmallCaliber.SM_Modern_Weapons_Projectile_SmallCaliber"));
    if (ensureMsgf(TracerMeshAsset.Succeeded(), TEXT("步枪测试示踪弹体缺少小口径弹体网格")))
    {
        TracerMesh->SetStaticMesh(TracerMeshAsset.Object);
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> TracerMaterialAsset(
        TEXT("/Game/_ThirdParty/Weapons/ModernWeapons/Assets/Projectiles/Materials/MI_EmissiveProjectile.MI_EmissiveProjectile"));
    if (ensureMsgf(TracerMaterialAsset.Succeeded(), TEXT("步枪测试示踪弹体缺少发光材质")))
    {
        TracerMesh->SetMaterial(0, TracerMaterialAsset.Object);
    }

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->SetUpdatedComponent(TracerMesh);
    ProjectileMovement->InitialSpeed = 12000.0f;
    ProjectileMovement->MaxSpeed = 12000.0f;
    ProjectileMovement->ProjectileGravityScale = 0.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->bInitialVelocityInLocalSpace = true;
}

void ABBBRifleDebugTracer::BeginPlay()
{
    Super::BeginPlay();

    const UWorld *World = GetWorld();
    if (!World || World->WorldType != EWorldType::PIE)
    {
        Destroy();
        return;
    }
}
