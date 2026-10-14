#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/Processors/BBBThrowableLifetimeProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Initialization/BBBThrowableInitializer.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
void FBBBThrowableLifetimeProcessor::Update(ABBBThrowable& Actor) const
{
    if (!Actor.HasAuthority())
    {
        return;
    }
    auto& Life = Actor.RuntimeData.Lifetime.State;
    auto& Explosion = Actor.RuntimeData.Explosion.State;
    if (!Life.bLaunched && Life.PendingDefinition)
    {
        Actor.Definition = Life.PendingDefinition;
        Actor.SetActorTransform(Life.PendingTransform);
        FBBBThrowableInitializer{}.Initialize(Actor);
        const auto& Launch = Actor.Definition->Launch;
        Actor.PhysicsBody->SetPhysicsLinearVelocity(Life.PendingTransform.GetUnitAxis(EAxis::X) * Launch.Speed
            + Life.PendingVelocity * Launch.VelocityInheritance);
        Actor.PhysicsBody->SetPhysicsAngularVelocityInDegrees(Life.PendingTransform.TransformVectorNoScale(Launch.AngularVelocity));
        Life.bLaunched = true;
        Life.LaunchTime = Actor.GetWorld()->GetTimeSeconds();
        Actor.ForceNetUpdate();
        UE_LOG(LogTemp, Log, TEXT("[BBBThrowable] Launched Authority=1 Mode=PredictiveInterpolation Position=%s"), *Actor.GetActorLocation().ToString());
    }
    if (!Life.bLaunched || !Actor.Definition || Explosion.Fact.bExploded)
    {
        return;
    }
    const double Age = Actor.GetWorld()->GetTimeSeconds() - Life.LaunchTime;
    const auto& Config = Actor.Definition->Explosion;
    if (Config.FuseSeconds > 0.0f && Age >= Config.FuseSeconds)
    {
        Explosion.bRequested = true;
    }
    if (Age >= Config.MaxLifetime && !Explosion.bRequested)
    {
        Actor.Destroy();
    }
}
