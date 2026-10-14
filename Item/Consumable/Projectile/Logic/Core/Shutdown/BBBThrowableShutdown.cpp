#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Shutdown/BBBThrowableShutdown.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "Components/SphereComponent.h"
void FBBBThrowableShutdown::Shutdown(ABBBThrowable& Actor) const
{
    Actor.PhysicsBody->OnComponentHit.RemoveAll(&Actor);
    Actor.RuntimeData.Parse.State = {};
    Actor.RuntimeData.Lifetime.State = {};
    Actor.RuntimeData.Explosion.State = {};
    Actor.RuntimeData.Network.State = {};
}
