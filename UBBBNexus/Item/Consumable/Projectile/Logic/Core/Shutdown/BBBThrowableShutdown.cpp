#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Shutdown/BBBThrowableShutdown.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
void FBBBThrowableShutdown::Shutdown(ABBBThrowable& Actor) const
{
    Actor.PhysicsBody->OnComponentHit.RemoveAll(&Actor);
    Actor.Trail->DeactivateImmediate();
    if (Actor.BurstEffect)
    {
        Actor.BurstEffect->DestroyComponent();
        Actor.BurstEffect = nullptr;
    }
    if (Actor.AreaEffect)
    {
        if (Actor.AreaEffect->PoolingMethod == ENCPoolMethod::ManualRelease)
        {
            Actor.AreaEffect->DeactivateImmediate();
            Actor.AreaEffect->ReleaseToPool();
        }
        else
        {
            Actor.AreaEffect->DestroyComponent();
        }
        Actor.AreaEffect = nullptr;
    }
    Actor.RuntimeData.Parse.State = {};
    Actor.RuntimeData.Lifetime.State = {};
    Actor.RuntimeData.Explosion.State = {};
    Actor.RuntimeData.Network.State = {};
}
