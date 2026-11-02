#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/NetworkSystem/Processors/BBBThrowableNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableNetworkComponent.h"
void FBBBThrowableNetworkProcessor::Update(ABBBThrowable& Actor) const
{
    const auto& State = Actor.RuntimeData.Explosion.ReadExplosionState();
    auto& Network = Actor.RuntimeData.Network.State;
    const auto& Life = Actor.RuntimeData.Lifetime.ReadLifetimeState();
    if (Actor.HasAuthority() && !State.Fact.bExploded && Life.Bounce.Serial > Network.PublishedBounceSerial)
    {
        Network.PublishedBounceSerial = Life.Bounce.Serial;
        Actor.Network->MulticastBounce(Life.Bounce);
    }
    if (Actor.HasAuthority() && State.Fact.bExploded && !Network.bPublished)
    {
        Network.bPublished = true;
        Actor.Network->Publish(State.Fact);
        Actor.ForceNetUpdate();
        Actor.SetLifeSpan(5.0f);
    }
}
