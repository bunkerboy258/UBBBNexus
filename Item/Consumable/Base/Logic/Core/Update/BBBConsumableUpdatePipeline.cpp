#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Update/BBBConsumableUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "Components/StaticMeshComponent.h"
void FBBBConsumableUpdatePipeline::ConfigureTick(ABBBConsumable &Actor)
{
    Actor.PrimaryActorTick.bCanEverTick = true;
    Actor.PrimaryActorTick.bStartWithTickEnabled = false;
    Actor.PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    Actor.PrimaryActorTick.EndTickGroup = TG_PostUpdateWork;
}

void FBBBConsumableUpdatePipeline::Update(ABBBConsumable &Actor) const
{
    if (Actor.IsInitialized())
    {
        UpdateInstance(Actor);
    }
}
