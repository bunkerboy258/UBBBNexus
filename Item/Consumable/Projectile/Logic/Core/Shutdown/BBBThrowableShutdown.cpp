#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Shutdown/BBBThrowableShutdown.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
void FBBBThrowableShutdown::Shutdown(ABBBThrowable& Actor) const
{
    Actor.PhysicsBody->OnComponentHit.RemoveAll(&Actor);
    Actor.Trail->DeactivateImmediate();
    Actor.RuntimeData.Parse.State = {};
    Actor.RuntimeData.Lifetime.State = {};
    Actor.RuntimeData.Explosion.State = {};
    Actor.RuntimeData.Network.State = {};
}
