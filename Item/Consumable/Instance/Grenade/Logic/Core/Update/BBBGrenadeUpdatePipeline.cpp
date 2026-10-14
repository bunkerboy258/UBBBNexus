#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/Core/Update/BBBGrenadeUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/BBBGrenadeConsumable.h"

void FBBBGrenadeUpdatePipeline::UpdateInstance(ABBBConsumable &BaseActor) const
{
    auto &Actor = static_cast<ABBBGrenadeConsumable &>(BaseActor);
    Actor.ParseSystem.Update();
    Actor.UseSystem.Update();
    Actor.NetworkSystem.Update();
}
