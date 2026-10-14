#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Initialization/BBBThrowableInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Update/BBBThrowableUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Shutdown/BBBThrowableShutdown.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABBBThrowable::ABBBThrowable()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    bAlwaysRelevant = true;
    SetReplicateMovement(true);
    SetPhysicsReplicationMode(EPhysicsReplicationMode::PredictiveInterpolation);
    PhysicsBody = CreateDefaultSubobject<USphereComponent>(TEXT("PhysicsBody"));
    SetRootComponent(PhysicsBody);
    PhysicsBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PhysicsBody->SetIsReplicated(true);
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(PhysicsBody);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Network = CreateDefaultSubobject<UBBBThrowableNetworkComponent>(TEXT("ThrowableNetwork"));
}
void ABBBThrowable::BeginPlay()
{
    Super::BeginPlay();
    FBBBThrowableInitializer{}.Initialize(*this);
}
void ABBBThrowable::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBThrowableUpdatePipeline{}.Update(*this);
}
void ABBBThrowable::EndPlay(const EEndPlayReason::Type Reason)
{
    FBBBThrowableShutdown{}.Shutdown(*this);
    Super::EndPlay(Reason);
}
void ABBBThrowable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABBBThrowable, Definition);
}
void ABBBThrowable::OnRep_Definition()
{
    FBBBThrowableInitializer{}.Initialize(*this);
}
void ABBBThrowable::OnPhysicsHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, FVector Impulse, const FHitResult& Hit)
{
    if (HasAuthority() && Definition && Definition->Explosion.bOnImpact)
    {
        SubmitInput(FBBBThrowableDetonateLocalControlPacket{});
    }
}
