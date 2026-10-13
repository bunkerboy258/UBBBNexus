#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/Core/Update/BBBGrenadeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"

void FBBBGrenadeUpdatePipeline::Update(ABBBGrenadeConsumable &Actor) const
{
    Actor.ParseSystem.Update();
    Actor.UseSystem.Update();
    Actor.NetworkSystem.Update();
}
