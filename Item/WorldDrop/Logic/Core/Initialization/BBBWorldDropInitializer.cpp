#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Initialization/BBBWorldDropInitializer.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Network/BBBWorldDropNetworkComponent.h"
#include "Components/SphereComponent.h"
#include "Components/MaterialBillboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/Core/Update/BBBWorldDropUpdatePipeline.h"
void FBBBWorldDropInitializer::Construct(ABBBWorldDropItem &Drop)
{
    Drop.PrimaryActorTick.bCanEverTick = true;
    Drop.bReplicates = true;
    Drop.SetPhysicsReplicationMode(EPhysicsReplicationMode::PredictiveInterpolation);
    Drop.SetReplicateMovement(true);
    Drop.Body = Drop.CreateDefaultSubobject<USphereComponent>(TEXT("DropBody"));
    Drop.SetRootComponent(Drop.Body);
    Drop.Body->InitSphereRadius(8.0f);
    Drop.Body->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Drop.Body->SetCollisionObjectType(ECC_WorldDynamic);
    Drop.Body->SetCollisionResponseToAllChannels(ECR_Ignore);
    Drop.Body->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Drop.Body->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    Drop.Body->SetLinearDamping(1.0f);
    Drop.Body->SetAngularDamping(2.0f);
    Drop.Icon = Drop.CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("DropIcon"));
    Drop.Icon->SetupAttachment(Drop.Body);
    Drop.Icon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Drop.Icon->SetCastShadow(false);
    Drop.Icon->SetTranslucentSortPriority(1);
    Drop.Beam = Drop.CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DropBeam"));
    Drop.Beam->SetupAttachment(Drop.Body);
    Drop.Beam->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Drop.Beam->SetCastShadow(false);
    Drop.Beam->SetReceivesDecals(false);
    Drop.Beam->SetAffectDynamicIndirectLighting(false);
    Drop.Beam->SetVisibleInRayTracing(false);
    Drop.Beam->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane")));
    Drop.GroundGlow = Drop.CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DropGroundGlow"));
    Drop.GroundGlow->SetupAttachment(Drop.Body);
    Drop.GroundGlow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Drop.GroundGlow->SetCastShadow(false);
    Drop.GroundGlow->SetReceivesDecals(false);
    Drop.GroundGlow->SetAffectDynamicIndirectLighting(false);
    Drop.GroundGlow->SetVisibleInRayTracing(false);
    Drop.GroundGlow->SetStaticMesh(Drop.Beam->GetStaticMesh());
    Drop.Network = Drop.CreateDefaultSubobject<UBBBWorldDropNetworkComponent>(TEXT("DropNetwork"));
}
void FBBBWorldDropInitializer::Initialize(ABBBWorldDropItem &Drop)
{
    const auto &Config = Drop.Config;
    Drop.RuntimeData.Drop.PresentationState = {};
    Drop.Body->SetSphereRadius(FMath::Max(1.0f, Config.Radius));
    Drop.Icon->SetVisibility(false);
    Drop.Beam->SetVisibility(false);
    Drop.GroundGlow->SetVisibility(false);
    Drop.Body->SetSimulatePhysics(Drop.HasAuthority());
    if (Drop.HasAuthority())
    {
        Drop.Body->SetPhysicsLinearVelocity(Config.InitialVelocity);
        auto &Snapshot = Drop.RuntimeData.Drop.DropState.Snapshot;
        Snapshot = {};
        Snapshot.DropId = FGuid::NewGuid();
        Snapshot.Definition = Config.Definition;
        Snapshot.Quantity = Config.Quantity;
        Snapshot.Revision = 1;
    }
}
void FBBBWorldDropInitializer::Preview(ABBBWorldDropItem &Drop)
{
    if (!Drop.GetWorld() || Drop.GetWorld()->IsGameWorld())
    {
        return;
    }
    auto &Snapshot = Drop.RuntimeData.Drop.DropState.Snapshot;
    Snapshot.Definition = Drop.Config.Definition;
    Snapshot.Quantity = Drop.Config.Quantity;
    Snapshot.bLanded = true;
    Drop.RuntimeData.Drop.PresentationState = {};
    Drop.Body->SetSphereRadius(FMath::Max(1.0f, Drop.Config.Radius));
    FBBBWorldDropUpdatePipeline::Update(Drop);
}
