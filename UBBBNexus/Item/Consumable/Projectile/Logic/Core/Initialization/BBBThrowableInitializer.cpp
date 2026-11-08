#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Initialization/BBBThrowableInitializer.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
void FBBBThrowableInitializer::Initialize(ABBBThrowable& Actor) const
{
    Actor.PhysicsBody->OnComponentHit.AddUniqueDynamic(&Actor, &ABBBThrowable::OnPhysicsHit);
    if (!Actor.Definition)
    {
        return;
    }
    const auto& Config = Actor.Definition->Physics;
    Actor.PhysicsBody->SetSphereRadius(Config.Radius);
    Actor.PhysicsBody->SetCollisionProfileName(Config.CollisionProfile);
    if (!Config.bCollideWithPawns)
    {
        Actor.PhysicsBody->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    }
    Actor.PhysicsBody->SetNotifyRigidBodyCollision(true);
    Actor.PhysicsBody->SetPhysMaterialOverride(Config.Material);
    Actor.PhysicsBody->SetMassOverrideInKg(NAME_None, Config.MassKg);
    Actor.PhysicsBody->SetLinearDamping(Config.LinearDamping);
    Actor.PhysicsBody->SetAngularDamping(Config.AngularDamping);
    Actor.PhysicsBody->SetEnableGravity(Config.bEnableGravity);
    Actor.PhysicsBody->SetUseCCD(Config.bCCD);
    Actor.Mesh->SetStaticMesh(Actor.Definition->Presentation.Mesh);
    Actor.Mesh->SetRelativeTransform(Actor.Definition->Presentation.MeshTransform);
    if (Actor.GetNetMode() != NM_DedicatedServer && Actor.Definition->Presentation.TrailEffect &&
        Actor.Trail->GetAsset() != Actor.Definition->Presentation.TrailEffect)
    {
        Actor.Trail->SetAsset(Actor.Definition->Presentation.TrailEffect);
        Actor.Trail->Activate(true);
    }
    Actor.PhysicsBody->SetSimulatePhysics(true);
}
