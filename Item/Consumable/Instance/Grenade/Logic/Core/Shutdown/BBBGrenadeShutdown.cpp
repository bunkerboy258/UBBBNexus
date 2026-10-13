#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/Core/Shutdown/BBBGrenadeShutdown.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"

void FBBBGrenadeShutdown::Shutdown(ABBBGrenadeConsumable &Actor) const
{
    Actor.SetActorTickEnabled(false);
}
