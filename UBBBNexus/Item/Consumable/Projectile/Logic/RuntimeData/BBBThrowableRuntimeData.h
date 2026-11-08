#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ParseSystem/DomainData/BBBThrowableParseDomainState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/LifetimeSystem/DomainData/BBBThrowableLifetimeDomainState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ExplosionSystem/DomainData/BBBThrowableExplosionDomainState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/NetworkSystem/DomainData/BBBThrowableNetworkDomainState.h"
/** 唯一投掷物业务黑板 */
struct FBBBThrowableRuntimeData final
{
    FBBBThrowableParseDomainState Parse;
    FBBBThrowableLifetimeDomainState Lifetime;
    FBBBThrowableExplosionDomainState Explosion;
    FBBBThrowableNetworkDomainState Network;
};
