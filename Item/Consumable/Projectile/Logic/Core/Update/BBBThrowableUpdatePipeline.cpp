#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/Core/Update/BBBThrowableUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/BBBThrowableLifetimeSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/BBBThrowableExplosionSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/NetworkSystem/BBBThrowableNetworkSystem.h"
void FBBBThrowableUpdatePipeline::Update(ABBBThrowable& Actor) const
{
    FBBBThrowableParseSystem{}.Update(Actor);
    FBBBThrowableLifetimeSystem{}.Update(Actor);
    FBBBThrowableExplosionSystem{}.Update(Actor);
    FBBBThrowableNetworkSystem{}.Update(Actor);
}
